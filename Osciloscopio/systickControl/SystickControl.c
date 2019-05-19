#include "registros.h"

SysTickIntervalo* listasIntervalos;
callback call;

void iniciarSystick(void)
{
	//STRELOAD contiene el límite al cual quiero reiniciar el timer
	//TODO es por parametro
	*STRELOAD = 99999UL; //Recarga cada 1 milisegundo. 100 mil porque el micro anda a 100 millones. La division da 1000 que son las milesimas en un seg

	//STCTRL es el registro para controlar el Systick - 0xE000E010
	/*
	 bit 0 ENABLE : System Tick counter enable. When 1, the counter is enabled. When 0, the counter is disabled.
	 bit 1 TICKINT: System Tick interrupt enable. When 1, the System Tick interrupt is enabled. When 0, the System Tick interrupt is disabled. When enabled, the interrupt is generated when the System Tick counter counts down to 0.
	 bit 2 CLKSOURCE System Tick clock source selection. When 1, the CPU clock is selected. When 0, the external clock pin (STCLK) is selected
	 */
	*STCTRL |= ((1 << 0) | (1 << 1) | (1 << 2));

	/*
	 * ALTERNATIVA
	 * uint32_t conf = 0b111;
	 * *STCTRL |= conf;
	 */

	//STCURR lleva la cuenta de ticks(máximo de 24 bits)
	*STCURR &= ~(0xFFFFFF << 0);	//Cargando con cero limpio y provoco el disparo de una interrupcion

	// Inicio la lista de funciones en null
	listasIntervalos = 0;
}

/**
 * Agrega una nueva funcion al systick
 * @param funcion funcion que ejecutara
 * @param ms cada cuantas milesimas de segundo se ejecuta
 * @param repetir Boolean que define si la funcion se ejecutará una vez o de forma cíclica
 * @return el nodo adherido a la lista del systick
 */
SysTickIntervalo* ejecutarFuncion(callback funcion, uint32_t ms, uint8_t repetir)
{

	// Alojo memoria para un nuevo nodo en la lista de funciones del systick
	SysTickIntervalo* intervalo = (SysTickIntervalo*) malloc(
			sizeof(SysTickIntervalo));

	intervalo->intervalo = ms;	// Seteo el intervalo de ejecucion de la funcion
	intervalo->contador = ms;	// Comienzo con el contado en 0 (Provoca que se ejecute en la proxima interrupcion del systick)
	intervalo->funcion = funcion;	// Seteo la funcion a ejecutar
	intervalo->repetirEjecucion = repetir;
	intervalo->siguiente = 0;	// Puntero a null (Es el ultimo de la lista)

	// Controlo si es el primero o debo recorrer la lista hasta llegar al final
	if (listasIntervalos == 0)
		listasIntervalos = intervalo;
	else
	{
		SysTickIntervalo* aux = listasIntervalos;

		while (aux->siguiente != 0)
			aux = aux->siguiente;

		aux->siguiente = intervalo;
	}

	// Retorno el intervalo para poder ser controlado externamente por quien agrego la nueva funcion
	return intervalo;
}

/**
 * Elimina una funcion del systick
 * @param intervalo nodo a ser borrado
 * @return booleano si lo encontro y borro o no
 */
int deleteFuncion(SysTickIntervalo* intervalo)
{
	SysTickIntervalo* aux = listasIntervalos;
	SysTickIntervalo* anterior = 0;

	// Recorro la lista hasta encontrar el nodo o llegar al final de la lista
	while(aux != 0 && aux != intervalo){
		anterior = aux;
		aux = aux->siguiente;
	}

	// Controlo si lo encontro o ese nodo no estaba en la lista
	if(aux != 0){
		// Reemplazo la lista según si era el primero o no
		if(aux == listasIntervalos)
			listasIntervalos = aux->siguiente;
		else
			anterior->siguiente = aux->siguiente;

		// Desalojo memoria
		free(aux);

		// Retorno true para indicar que fue borrado
		return 1;
	}

	// Retorno false para indicar que el nodo no estaba en la lista
	return 0;

}

/**
 * Analiza si hay que ejecutar la funcion y actualiza su contador
 * @param intervalo nodo a ser analizado
 * @return void no retorna nada
 */
inline void analizarIntervalo(SysTickIntervalo *intervalo)
{

	// Si llego a 0 es momento de ejecutar si no descontar una unidad el contador
	if (intervalo->contador == 0)
	{
		// Ejecuto la funcion
		call = intervalo->funcion;
		call();//El llamado a la función recibe su propio nodo porque es capaz de modificarse asi mismo

		//Si no debo repetir la ejecucion entonces la elimino
		if(intervalo->repetirEjecucion == NO_REPETIR)
			deleteFuncion(intervalo);
		else//Si debo repetir la ejecucion de la funcion, reinicio el contador
			intervalo->contador = intervalo->intervalo;
	}
	else
		intervalo->contador--;
}

/*
 * Interrupcion del systick
 * @param void no recive parametros
 * @return void no retorna nada
 */
void SysTick_Handler(void)
{
	SysTickIntervalo* aux = listasIntervalos;

	// Recorro la lista actualizando y ejecutando (en caso de contador = 0) las funciones
	while (aux != 0)
	{
		analizarIntervalo(aux);
		aux = aux->siguiente;
	}
}
