/*
 * SystickControl.h
 *
 *  Created on: 2 ago. 2018
 *      Author: DvdZmd
 */

#ifndef SYSTICKCONTROL_H_
#define SYSTICKCONTROL_H_

#define REPETIR 1
#define NO_REPETIR 0



/*
	UM10360
	Chapter 23: LPC176x/5x System Tick Timer
*/
//Registros del SysTick:
//STCTRL: Registro de control del SysTick:
#define STCTRL		((uint32_t*) 0xE000E010UL)
//STRELOAD: Cuando el contador del timer alcance este valor, se reiniciará a 0 y de estar configurada, ejecutará una interrupción
#define STRELOAD	((uint32_t*) 0xE000E014UL)
//STCURR: Lleva el conteo del SysTick, es leible, escribír en ella probocaría su reinicio
#define STCURR		((uint32_t*) 0xE000E018UL)
//0xE000E01CUL: Registro de calibracion del SysTick:
#define STCALIB		((uint32_t*) 0xE000E01CUL)

//Estructura para el control y la ejecucion de funciones en el Systick_Handler()
typedef struct _SysTickIntervalo
{
	uint32_t intervalo;//tiempo en milisegundos para ejecutar la funcion
	uint32_t contador;//contador interno para saber cuando ejecutar
	callback funcion;//puntero a funcion que ejecutar en la interrupcion (callback)
	uint8_t repetirEjecucion;//Opción de configuración, si el callback debe ser ejecutado una sola vez o repetidas veces respetando el intervalo(Se recomienda usar los defines REPETIR NO_REPETIR)
	struct _SysTickIntervalo *siguiente;//Puntero al siguiente nodo de la lista de funciones a ejecutar
} SysTickIntervalo;

void iniciarSystick(void);
void analizarIntervalo(SysTickIntervalo*);
int deleteFuncion(SysTickIntervalo*);
SysTickIntervalo* ejecutarFuncion(callback, uint32_t, uint8_t);


#endif /* SYSTICKCONTROL_H_ */
