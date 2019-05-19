/*
 * gpioControl.h
 *
 *  Created on: 2 ago. 2018
 *      Author: DvdZmd
 */

#ifndef GPIOCONTROL_H_
#define GPIOCONTROL_H_

//Estados de FIODIR:
#define		SALIDA	1
#define		ENTRADA	0


//Estructura para manejar los GPIOs:
//Estados de PINSEL:
#define		GPIO	0
#define		FUNC1	1
#define		FUNC2	2
#define		FUNC3	3

//Estados de PINMODE
#define		PULLUP 		0
#define		REPEAT 		1
#define		NONE 		2
#define		PULLDOWN 	3


typedef struct
{
	uint32_t	FIODIR;
	uint32_t	RESERVED[3];
	uint32_t 	FIOMASK;
	uint32_t 	FIOPIN;
	uint32_t 	FIOSET;
	uint32_t 	FIOCLR;
}gpio_t;

#define		 FIO2PIN0 ( ( uint32_t*) 0x2009C054UL)

//0x2009C000UL : Direccion de inicio de los registros de GPIOs
#define		GPIOs		( ( gpio_t*) 0x2009C000UL )

//0x4002C000UL : Direccion de inicio de los registros PINSEL
#define		PINSEL		( (uint32_t*) 0x4002C000UL )

//0x4002C040UL : Direccion de inicio de los registros de modo de los pines del GPIO
#define		PINMODE		( (uint32_t*) 0x4002C040UL )


void gpioSet(uint32_t, uint32_t,uint32_t);
unsigned int gpioRead(uint32_t, uint32_t);
void gpioDir(uint32_t,uint32_t,uint32_t);
void gpioFuntion(uint32_t, uint32_t, uint32_t);
void gpioMode(uint32_t,uint32_t,uint32_t);
void gpioConfig(uint32_t,uint32_t,uint32_t, uint32_t, uint32_t);

//Bus Paralelo
void IniciarBusParalelo(uint32_t, uint32_t);
void escribirBusParalelo(uint32_t, uint32_t, uint8_t);

#endif /* GPIOCONTROL_H_ */
