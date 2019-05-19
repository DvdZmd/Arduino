
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef void (*callback)(void);

#ifndef REGS_H_
#define REGS_H_


//0x400FC1A8UL : Direccion de inicio de los registros de seleccion de los clks de los dispositivos:
#define		PCLKSEL		( (uint32_t*) 0x400FC1A8UL )


//0x400FC0C4UL : Direccion de inicio del registro de habilitaci�n de dispositivos:
#define		PCONP		( (uint32_t*) 0x400FC0C4UL )

//Registros PCLKSEL
#define		PCLKSEL0	PCLKSEL[0]
#define		PCLKSEL1	PCLKSEL[1]

#define 	SYSTICK		( (uint32_t *) 0xE000E010UL )


// Nested Vectored Interrupt Controller (NVIC)
// 0xE000E100UL : Direccion de inicio de los registros de habilitación (set) de interrupciones en el NVIC:
#define		ISER0		( ( unsigned int  * ) 0xE000E100UL )
// 0xE000E180UL : Direccion de inicio de los registros de deshabilitacion (clear) de interrupciones en el NVIC:
#define		ICER		( ( unsigned int  * ) 0xE000E180UL )

#define UN_MILISEGUNDO 1000
#define UN_MICROSEGUNDO 1
#define HABILITAR_INTERRUPCION 1
#define INHABILITAR_INTERRUPCION 0


#define LOW 0
#define HIGH 1
#define ACTIVO 1
#define INACTIVO 0
#define BAJO 0
#define ALTO 1


/*
 * 8-/9-/16-/18-bit MCU 8080-I series parallel interface.
 * The chip-select CSX (active low)  is used to  enable or disable LCD chip.
 * The RESX (active  low) is  an  external reset signal.
 * WRX is the parallel data write strobe, RDX is the parallel data read strobe
 * and D[17:0] is parallel data bus.
 */
#define CS 2,8
#define WR 2,10
#define RD 0,21
#define RS 0,22
#define RESX 0,3

/*When the Data/Command (RS) signal is driven to low level, then input data on the interface is interpreted as command.
The D/CX signal also can be pulled high level when the data on the interface is RAM data or command parameter.*/

/*
 * Lee la data de entrada al rising edge del WRX.
 * RS define si se ingresa dato o comando.
 * Cuando RS=’1’, DB[17:0] bits son display RAM data o parametros de comados. Cuando RS=’0’, D[17:0] bits son comandos.
 */

#define DB0 2,0
#define DB1 2,1
#define DB2 2,2
#define DB3 2,3
#define DB4 2,4
#define DB5 2,5
#define DB6 2,6
#define DB7 2,7

#define BUS_PARALELO 2,0


#define LIMITE_INFERIOR (estableX-estableX)
#define LIMITE_SUPERIOR (estableY*2)
#define ILI 0
#define SSD 1

#include "SystickControl.h"
#include "gpioControl.h"
#include "Oscilador.h"
#include "timer.h"
#include "displayLCD.h"
#include "adc.h"
#include "serial.h"
#include "debug.h"
#include "app.h"
#include "teclado.h"
#include "serial.h"

#endif /* REGS_H_ */
