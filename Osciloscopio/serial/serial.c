//El pin mode (PINSEL) corresponde a TXDn y RXDn siendo n la UART 1 2 3 o 4
#include "registros.h"

uint8_t buff_rx[TAMANIO_BUF];
uint8_t buff_tx[TAMANIO_BUF];

uint32_t in_tx = 0;
uint32_t out_tx = 0;

uint32_t in_rx = 0;
uint32_t out_rx = 0;

volatile uint8_t tx_curso = 0;

#define RX 2
#define TX 1
uint8_t uartPCONP[] =
	{ 3, 4, 24, 25 };

uint32_t * uartLCR[] =
	{ DIR_U0LCR, DIR_U1LCR, DIR_U2LCR, DIR_U3LCR };

uint32_t * uartFCR[] =
	{ DIR_U0FCR, DIR_U0FCR, DIR_U2FCR, DIR_U3FCR }; //Esta dos veces DIR_U0FCR porque la 1 no está "defineada" TODO

uint32_t * uartIER[] =
	{ DIR_U0IER, DIR_U1IER, DIR_U2IER, DIR_U3IER };

uint8_t uartPCLKSEL[] =
	{ 6, 8, 16, 18 };

uint32_t * uartDLM[] =
	{ DIR_U0DLM, DIR_U1DLM, DIR_U2DLM, DIR_U3DLM };

uint32_t * uartDLL[] =
	{ DIR_U0DLL, DIR_U1DLL, DIR_U2DLL, DIR_U3DLL };

uint32_t * uartIRR[] =
	{ DIR_U0IIR, DIR_U1IIR, DIR_U2IIR, DIR_U3IIR };

uint32_t * uartTHR[] =
	{ DIR_U0THR, DIR_U1THR, DIR_U2THR, DIR_U3THR };

uint32_t * uartRBR[] =
	{ DIR_U0RBR, DIR_U1RBR, DIR_U2RBR, DIR_U3THR };

void
configurarPinesUart (uint8_t uart)
{

	switch (uart)
	{
		case 0:
			gpioConfig (0, 2, 1, PULLDOWN, SALIDA);
			gpioConfig (0, 3, 1, PULLDOWN, ENTRADA);
			break;
		case 1:
			gpioConfig (0, 15, 1, PULLDOWN, SALIDA);
			gpioConfig (0, 16, 1, PULLDOWN, ENTRADA);
			break;
		case 2:
			gpioConfig (0, 10, 1, PULLDOWN, SALIDA);
			gpioConfig (0, 11, 1, PULLDOWN, ENTRADA);
			break;
		case 3:
			gpioConfig (0, 0, 2, PULLDOWN, SALIDA);
			gpioConfig (0, 1, 2, PULLDOWN, ENTRADA);
			break;

	}

}

void
Init_UART (uint32_t uart)
{
	if (uart > 3)
		return;

	configurarPinesUart (uart);

	*PCONP |= (1 << uartPCONP[uart]);	//Enciendo la UART0 en el PCONP
	*uartFCR[uart] |= 0x7;	//Enableo la FIFO y reseteo las TX y RX FIFO
	*uartLCR[uart] = 0x83; //DLAB=1, sin paridad, 1 bit stop, 8 bit datos.

	//Seteamos el PCLK en CCLK/4 (25 Mhz)
	PCLKSEL[uart == 2 || uart == 3] &= ~(3 << uartPCLKSEL[uart]);

	/*
	 Divisor Latch MSB. Most significant byte of the baud
	 rate divisor value. The full divisor is used to generate a
	 baud rate from the fractional rate divider.
	 */
	*uartDLM[uart] = 0;

	/*
	 Divisor Latch LSB. Least significant byte of the baud
	 rate divisor value. The full divisor is used to generate a
	 baud rate from the fractional rate divider.
	 */
	*uartDLL[uart] = 0x51;//19200
	//(25000000 / 16) / 9600 = 162.76 => 163 = 0xA2		//25Mhz, baudrate=9600.

	//U0LCR &= ~(1<<7);	//DLAB = 0
	*uartLCR[uart] &= 0x7F;

	//*uartLCR[uart] &= 0x03;
	*uartIER[uart] = 0x03;  //Habilitamos interrupciones ERBFI y ETBEI

	//ISER0 |= (1<<ISE_UART0);	//Habilito interrupción NVIC //estaba puesto como &=
	*ISER0 |= 1 << (5 + uart);
}

/*
 * Llega un dato por serial y lo guardo en mi buffer para leerlo donde quiera desde mi código
 */
void
cola_in_rx (uint8_t dato)
{
	buff_rx[in_rx] = dato;

	//El código de abajo fue la primer version, pero usando % se puede hacer en una sola linea
	in_rx = (in_rx + 1) % TAMANIO_BUF;
}

uint16_t
cola_out_rx (void)
{ //Se usan dos colas circulares para el buffer, una para RX y otra para TX, donde cada cola se comparte para in y para out

	uint8_t dato;

	if (in_rx == out_rx) //Si los índices de in y out son iguales
		return -1; //no hay datos

	/*Leo el buffer que fue cargado en la interrupción de la UART filtrado a RX
	 * incremento el índice de lectura para saber cuando leí todos los datos
	 */
	dato = buff_rx[out_rx];
	out_rx++;
	if (out_rx >= TAMANIO_BUF)
		out_rx = 0;

	return (uint16_t) dato;
}

void
cola_in_tx (uint8_t dato)
{ //Cargo mi buffer personal que voy a usar para pasarle los datos de transmisión a la UART
	buff_tx[in_tx] = dato;
	in_tx++;
	if (in_tx >= TAMANIO_BUF)
		in_tx = 0;
}


/*
 * Lee un caracter del buffer para asignarselo al registro de transmición de la UART
 */
uint16_t
cola_out_tx (void)
{ //Se usan dos colas circulares para el buffer, una para RX y otra para TX, donde cada cola se comparte para in y para out

	uint8_t dato;

	if (in_tx == out_tx) //Si los índices de in y out son iguales
		return -1; //no hay datos

	/*Leo el dato del buffer e incremento el índice. El buffer se carga a mano en la programación,
	 * la idea es cargar un valor en el registro de transmisión y que la UART se encargue de transmitirlo
	 */
	dato = buff_tx[out_tx];
	out_tx++;

	if (out_tx >= TAMANIO_BUF) //reinicio el buffer
		out_tx = 0;

	return (uint16_t) dato;
}

uint16_t
UART_RECIBIR (void)
{
	return cola_out_rx ();
}

uint8_t
UART_HayDatos ()
{
	return UART_RECIBIR () != 65535;
}

//Entre estas funciones se comparte el buffer
void
UART_Enviar (uint8_t *cadena, uint32_t tamanio)
{
	//cargar cadena en buffer
	uint8_t i;

	for (i = 0; i < tamanio; i++) //Cargo mi buffer para transmitir
		cola_in_tx (cadena[i]);

	if (tx_curso == 0)
	{ //Si la UART no está ocupada, cargo datos de transmisión
		*uartTHR[3] = cola_out_tx (); //Escribir el  Transmit Holding Register... ¿Dispara la interrupción? //TODO verificar

	}
}

void
UART2_IRQHandler (void)
{
	UART_IRQ_HANDLER (2);
}

void
UART3_IRQHandler (void)
{
	UART_IRQ_HANDLER (3);
}

void
UART_IRQ_HANDLER (uint8_t uart) //Entra por Rx o Tx
{ //TODO debuguear en que momento entra en la interrupción, hacer pruebas de transmitir y de recibír
	uint32_t temp;
	do
	{
		temp = *uartIRR[uart];  //Interrup ID Register: Defines wich interrup(s) are pending
		switch ((temp >> 1) & 0x03)
		{
			case RX: //DEFINE RX 2
				UART_RX_INTERRUPCION (uart);
				break;
			case TX: //DEFINE TX 1
				UART_TX_INTERRUPCION (uart);
				break;
		}
	}
	while ((temp & 1) == 0);
}

void
UART_TX_INTERRUPCION (uint8_t uart)
{ //Me avisaron que tengo que transmitir datos

	uint16_t dato = cola_out_tx (); //Leo un dato del buffer para transmitirlo

	if (dato != 65535)//Si hay datos...
	{
		*uartTHR[uart] = (uint8_t) dato; //Cargo el dato en el registro para que la UART se encargue de transmitirlo
		tx_curso = 1; //Levanto el flag de ocupado
	}
	else
	{
		tx_curso = 0;
	}

}

void
UART_RX_INTERRUPCION (uint8_t uart)
{ //Entra un dato por interrupcion
	cola_in_rx (*uartRBR[uart]);
}

