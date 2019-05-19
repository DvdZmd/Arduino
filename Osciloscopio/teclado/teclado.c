#include "registros.h"

volatile uint8_t teclaValida;

void tecladoAntirebote(SysTickIntervalo *intervalo){

	uint8_t teclaLeida = gpioRead(0,15);//Leo directamente el estado del pin me interesa filtrar el rebore
	static uint8_t teclaAnterior = 0;
	static uint16_t contador = 0;


	if(teclaLeida == teclaAnterior){

		if(contador == CANTIDAD_ESTABLE)
		{
			teclaValida = teclaLeida;
		}
		else
			contador++;
	}
	else
		contador = 0;

	teclaAnterior = teclaLeida;

}

uint8_t leerTeclado()
{
	return teclaValida;
}
