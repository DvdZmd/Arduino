/*
 * gpioControl.c
 *
 *  Created on: 2 ago. 2018
 *      Author: DvdZmd
 */

#include "registros.h"

void gpioSet(uint32_t puerto, uint32_t pin, uint32_t estado) {
	//Para setear el estado de un pin también se puede usar el registro FIOPIN
	if (estado == 1)
		GPIOs[puerto].FIOSET = (1 << pin); //escribir un 1 en FIOSET pone en HIGH el pin
	else if (estado == 0)
		GPIOs[puerto].FIOCLR = (1 << pin); //escribir un 1 en FIOCLR pone en LOW el pin
}

unsigned int gpioRead(uint32_t puerto, uint32_t pin) {
	if (GPIOs[puerto].FIOPIN & (1 << pin))
		return 1;
	else
		return 0;
}

void gpioDir(uint32_t puerto, uint32_t pin, uint32_t estado) {
	if (estado == 1)
		GPIOs[puerto].FIODIR |= (1 << pin);
	else
		GPIOs[puerto].FIODIR &= ~(1 << pin);

}

//The PINSEL registers control the functions of device pins
void gpioFuntion(uint32_t puerto, uint32_t pin, uint32_t funcion) {
	uint32_t* registroPuerto;
	if (pin < 16)
		registroPuerto = &PINSEL[puerto * 2];
	else
		registroPuerto = &PINSEL[(puerto * 2) + 1];

	*registroPuerto &= ~(3 << (pin % 16) * 2);
	*registroPuerto |= (funcion << (pin % 16) * 2);

	/*
	 * PINSEL originalmente se declara como puntero, pero acá lo estamos manipulando
	 * como si fuese un vector porque a fines prácticos es lo mismo.
	 * El método respetando a PINSEL como puntero podría ser de la siguiente manera:
	 *
	 * registroPuerto = PINSEL + (puerto*2);
	 * if(pin > 15)
	 * 		registroPuerto++;
	 */
	//Testear
}

void gpioMode(uint32_t puerto, uint32_t pin, uint32_t modo) { //Puerto: pin0:10 pin1:11 pin2:00 pin3:10 -- pin4:01 pin5:10 pin6:11 pin7:11
	unsigned int *registroPuerto;
	if (pin < 16)
		registroPuerto = &PINMODE[puerto * 2];

	else
		registroPuerto = &PINMODE[(puerto * 2) + 1];

	*registroPuerto &= ~(3 << (pin % 16) * 2);
	*registroPuerto |= (modo << (pin % 16) * 2);
}

/**
 * PUERTO
 * PIN
 * FUNCION
 * MODO
 * DIRECCION
 */
void gpioConfig(uint32_t puerto, uint32_t pin, uint32_t funcion, uint32_t modo,
		uint32_t direccion) {
	gpioFuntion(puerto, pin, funcion);
	gpioMode(puerto, pin, modo);
	if (funcion == GPIO)
		gpioDir(puerto, pin, direccion);

}

/*
 * Funciones para manejo de Bus Paralelo
 */

/**
 * Efectua el gpioConfig para un bus paralelo
 * @param puerto cual puerto utiliza (0,1,2,3)
 * @param pinInicial primer pin del bus paralelo, se espera que se haya configurado el bus por pines consecutivos
 * @return void no retorna nada
 */
void IniciarBusParalelo(uint32_t puerto, uint32_t pinInicial) {
	/*
	 * Uso el puerto 2 y los pines 0 a 7 para el bus de paralelo porque está contíguos en el LPC1769
	 */
	int i;
	for (i = 0; i < 8; i++) {
		gpioConfig(puerto, (pinInicial + i), GPIO, PULLUP, SALIDA);
		gpioSet(puerto, (pinInicial + i), HIGH);
	}

	gpioConfig(CS, GPIO, PULLUP, SALIDA);
	gpioConfig(RS, GPIO, PULLUP, SALIDA);
	gpioConfig(RD, GPIO, PULLUP, SALIDA);
	gpioConfig(WR, GPIO, PULLUP, SALIDA);
	gpioConfig(RESX, GPIO, PULLUP, SALIDA);

	gpioSet(CS, LOW);
	gpioSet(RS, LOW);
	gpioSet(WR, LOW);
	gpioSet(RD, HIGH);
	gpioSet(RESX, LOW);

}

void leerBusParalelo(uint32_t puerto, uint32_t pinInicial, uint8_t* lectura) { //Se usa pinInicial porque se espera que para un Bus Paralelo se usen los pines contiguos numéricamente en el puerto
	*lectura = 0;
	for (int i = 0; i < 8; i++) {
		*lectura |= (gpioRead(puerto, pinInicial + i) << i);
	}
}

/**
 * Escribe una secuencia de bits en el respectivo pin según el peso del bit
 * @param puerto cual puerto utiliza (0,1,2,3)
 * @param pinInicial primer pin del bus paralelo, se espera que se haya configurado el bus por pines consecutivos
 * @param escritura es el valor recibido para escribir sobre el bus, se recomienza usar una notación hexadecima ej: escribirBusParalelo(BUS, 0x8B)
 * @return void no retorna nada
 */
void escribirBusParalelo(uint32_t puerto, uint32_t pinInicial, uint8_t escritura) {

	GPIOs[puerto].FIOPIN &= ~(0xFF);
	GPIOs[puerto].FIOPIN |= escritura;
	delay(10);

	for (int i = 0; i < 8; i++) {
		if ((1 << i) & escritura)
			GPIOs[puerto].FIOSET = (1 << (pinInicial + i));
		else
			GPIOs[puerto].FIOCLR = (1 << (pinInicial + i));
	}

}

