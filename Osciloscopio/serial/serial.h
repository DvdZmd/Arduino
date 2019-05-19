/*
 * serial.h
 *
 *  Created on: 29 de ago. de 2017
 *      Author: DvdZmd
 */
#include "registros.h"

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

//UART0:
//0x4001000CUL : Registro de control FIFO de la UART0:
#define		DIR_U0FCR	( ( uint32_t  * ) 0x4000C008UL )
//0x4001000CUL : Registro de control de la UART0:
#define		DIR_U0LCR	( ( uint32_t  * ) 0x4000C00CUL )
//0x40010014UL : Registro de recepcion de la UART0:
#define		DIR_U0LSR		( ( uint32_t  * ) 0x4000C014UL )
//0x40010000UL : Parte baja del divisor de la UART0:
#define		DIR_U0DLL	( ( uint32_t  * ) 0x4000C000UL )
//0x40010004UL : Parte alta del divisor de la UART0:
#define		DIR_U0DLM	( ( uint32_t  * ) 0x4000C004UL )
//0x40010000UL : Registro de recepcion de la UART0:
#define		DIR_U0RBR		( ( uint32_t  * ) 0x4000C000UL )
//0x40010000UL : Registro de transmision de la UART0:
#define		DIR_U0THR		( ( uint32_t  * ) 0x4000C000UL )

#define     DIR_U0IER       ( ( uint32_t * ) 0x4000C004UL )
#define     DIR_U0IIR       ( ( uint32_t * ) 0x4000C008UL )

//UART1:
//0x4001000CUL : Registro de control de la UART1:
#define		DIR_U1LCR	( ( uint32_t  * ) 0x4001000CUL )
//0x40010014UL : Registro de recepcion de la UART1:
#define		DIR_U1LSR		( ( uint32_t  * ) 0x40010014UL )
//0x40010000UL : Parte baja del divisor de la UART1:
#define		DIR_U1DLL	( ( uint32_t  * ) 0x40010000UL )
//0x40010004UL : Parte alta del divisor de la UART1:
#define		DIR_U1DLM	( ( uint32_t  * ) 0x40010004UL )
//0x40010000UL : Registro de recepcion de la UART1:
#define		DIR_U1RBR	( ( uint32_t  * ) 0x40010000UL )
//0x40010000UL : Registro de transmision de la UART1:
#define		DIR_U1THR	( ( uint32_t  * ) 0x40010000UL )

#define     DIR_U1IER       ( ( uint32_t * ) 0x40010004UL )
#define     DIR_U1IIR       ( ( uint32_t * ) 0x40010008UL )

//UART2:
//0x40098008UL : Registro de control FIFO de la UART2:
#define		DIR_U2FCR	( ( uint32_t  * ) 0x40098008UL )
//0x4009800CUL : Registro de control de la UART2:
#define		DIR_U2LCR	( ( uint32_t  * ) 0x4009800CUL )
//0x40098014UL : Registro de recepcion de la UART2:
#define		DIR_U2LSR		( ( uint32_t  * ) 0x40098014UL )
//0x40098000UL : Parte baja del divisor de la UART2:
#define		DIR_U2DLL	( ( uint32_t  * ) 0x40098000UL )
//0x40098004UL : Parte alta del divisor de la UART2:
#define		DIR_U2DLM	( ( uint32_t  * ) 0x40098004UL )
//0x40098000UL : Registro de recepcion de la UART2:
#define		DIR_U2RBR		( ( uint32_t  * ) 0x40098000UL )
//0x40098000UL : Registro de transmision de la UART2:
#define		DIR_U2THR		( ( uint32_t  * ) 0x40098000UL )

//UART3:
//0x40098008UL : Registro de control FIFO de la UART3:
#define		DIR_U3FCR	( ( uint32_t  * ) 0x4009C008UL )
//0x4009800CUL : Registro de control de la UART3:
#define		DIR_U3LCR	( ( uint32_t  * ) 0x4009C00CUL )
//0x40098014UL : Registro de recepcion de la UART3:
#define		DIR_U3LSR		( ( uint32_t  * ) 0x4009C014UL )
//0x40098000UL : Parte baja del divisor de la UART3:
#define		DIR_U3DLL	( ( uint32_t  * ) 0x4009C000UL )
//0x40098004UL : Parte alta del divisor de la UART3:
#define		DIR_U3DLM	( ( uint32_t  * )  0x4009C004UL )
//0x40098000UL : Registro de recepcion de la UART3:
#define		DIR_U3RBR		( ( uint32_t  * ) 0x4009C000UL )
//0x40098000UL : Registro de transmision de la UART3:
#define		DIR_U3THR		( ( uint32_t  * ) 0x4009C000UL )


#define     DIR_U2IER       ( ( uint32_t * ) 0x40098004UL )
#define     DIR_U2IIR       ( ( uint32_t * ) 0x40098008UL )

#define     DIR_U3IER       ( ( uint32_t * )  0x4009C004UL )
#define     DIR_U3IIR       ( ( uint32_t * )  0x4009C008UL )

#define     DIR_U0IER       ( ( uint32_t * ) 0x4000C004UL )
#define     DIR_U0IIR       ( ( uint32_t * ) 0x4000C008UL )



#ifndef SERIAL_H_
#define SERIAL_H_

#define TAMANIO_BUF 32

void configurarPinesUart(uint8_t);
void cola_in_rx(uint8_t);
uint16_t cola_out_rx(void);
void cola_in_tx(uint8_t);
uint16_t cola_out_tx(void);
uint16_t UART_RECIBIR(void);
uint8_t UART_HayDatos();
void UART_Enviar(uint8_t*, uint32_t);
uint8_t UART_transmitiendo(void);
//void UARTx_Inicializacion(uint8_t);
void Init_UART(uint32_t);
void UART_IRQ_HANDLER(uint8_t);
void UART_TX_INTERRUPCION(uint8_t);
void UART_RX_INTERRUPCION(uint8_t);
/*void UART0_IRQHandler(void);
 void UART1_IRQHandler(void);
 void UART2_IRQHandler(void);
 void UART3_IRQHandler(void);*/

#endif /* SERIAL_H_ */
