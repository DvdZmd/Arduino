#include "registros.h"

timer_t *timers[] = {DIR_TIMER0, DIR_TIMER1, DIR_TIMER2, DIR_TIMER3}; //Lista de las direccciones de los timer (la estructura mirar en timer.h)
uint32_t TIMERPWR[] = {TIMER0PW, TIMER1PW, TIMER2PW, TIMER3PW}; //Bits del PCLKSEL para energisar los timer
timer_t* temporizador = TIMER_CERO;

timerFun TIMER0_fun[] = {0,0,0,0,0,0}; //Funciones que ejecuta el Timer0 (las primeras 4 son de match las ultimas 2 de capture). 0 es null (no ejecutar nada)
timerFun TIMER1_fun[] = {0,0,0,0,0,0}; //Funciones que ejecuta el Timer1 (las primeras 4 son de match las ultimas 2 de capture)
timerFun TIMER2_fun[] = {0,0,0,0,0,0}; //Funciones que ejecuta el Timer2 (las primeras 4 son de match las ultimas 2 de capture)
timerFun TIMER3_fun[] = {0,0,0,0,0,0}; //Funciones que ejecuta el Timer3 (las primeras 4 son de match las ultimas 2 de capture)
timerFun *TIMER_fun[] = {TIMER0_fun,TIMER1_fun,TIMER2_fun,TIMER3_fun}; //Vector de la posicion en memoria del vector de funciones de todos los timer
//*************************************************************************************************************************************//
//***************************************************Timers****************************************************************************//
//*************************************************************************************************************************************//

void delay(uint32_t micros)
{
	temporizador->TC = 0;

	while(temporizador->TC < micros)
	{

	}
}

/**
 * Inicializa un Timer
 * @param timer cual timer inicia (0,1,2,3)
 * @return void no retorna nada
 */
void InicializarTimer(uint32_t timer){

	// Controlo que no exceda la cantidad permitida
	if(timer > 3)
		return;

	// Me traigo local el timer a iniciar
	timer_t *selTimer = timers[timer];

	//Habilito clock power para el timer indicado
	*PCONP |= (1<<TIMERPWR[timer]);

	//Elijo el clock (No estan todos contiguos)
	if(timer < 2)
		PCLKSEL0 |= (1<<(2+2*timer));
	else
		PCLKSEL1 |= (1<<(12+2*(timer-2)));

	// Prescaler a 100, como se selecciona el relog de 100Mhz da como resultado 1Mhz, es decir 1 micro segundo
	selTimer->PR = 100 - 1; // Cada cuantos tick avanza el timer counter
	selTimer->TC = 0; // Reseteo a 0 por las dudas

	// Habilito el timer para que comienza a funcionar
	selTimer->TCR = (1<<0);

	// Aseguro que este bit este 0. Setearlo a 1 resetea el timer counter y el prescale counter
	selTimer->TCR &= ~(1<<1);

	// Habilito interrupcion del timer
	*ISER0 |= ( 1 << (1+timer) ) ;
}

/**
 * Inicializa un Match
 * @param timer cual timer inicia (0,1,2,3)
 * @param match cual match inicia (0,1,2,3)
 * @param us microsegundo en el cual se ejecutara l match
 * @param rs un valor en 1 indica que el match debe reiniciar el timer
 * @param funcion a ejecutar cuando el match lanze la interrupcion
 * @return void no retorna nada
 */
void InicializarMatch(uint32_t timer, uint32_t match, uint32_t us, uint32_t rs, timerFun funcion){

	// Controlo que no exceda la cantidad permitida
	if(timer > 3 || match > 3)
		return;

	// Me traigo local el timer a configurar
	timer_t *selTimer = timers[timer];

	// Agrego la funcion a ejecutar cuando el match provoque una interrupcion
	TIMER_fun[timer][match] = funcion;

	//Habilito el match
	selTimer->MCR |= (1<<(3*match));

	//habilito reseteo cuando matchea
	if(rs)
		selTimer->MCR |= (1<<(1 + 3*match));
	else
		selTimer->MCR &= ~(1<<(1 + 3*match));

	// Valor que compara constantemente el timer para saber si llego al match
	(&selTimer->MR0)[match] = us;

}

void deshabilitarMatch(uint8_t timer, uint8_t match)
{
	timers[timer]->MCR &= ~(1<<(3*match));
}

inline void habilitarMatch(uint8_t timer, uint8_t match)
{
	timers[timer]->MCR |= (1<<(3*match));
}

inline void habilitarTimer(uint8_t timer){
	timers[timer]->TCR |= (1<<0);
}

inline void deshabilitarTimer(uint8_t timer){
	timers[timer]->TCR &= ~(1<<0);
}

inline void reiniciarTimerCounter(uint8_t timer)
{
	timers[timer]->TC = 0;
}

inline uint32_t timerCount(uint8_t timer)
{
	return timers[timer]->TC;
}

inline timer_t * getTimer(uint8_t timer)
{
	return timers[timer];
}

/**
 * Inicializa un Capture
 * @param timer cual timer inicia (0,1,2,3)
 * @param capture cual capture inicia (0,1,2,3)
 * @param funcion a ejecutar cuando el capture lanze la interrupcion
 * @return void no retorna nada
 */
void Inicializar_Capture(uint32_t timer, uint32_t capture, timerFun funcion){

	// Controlo que no exceda la cantidad permitida
	if(timer > 3 || capture > 1)
		return;

	// Me traigo local el timer a configurar
	timer_t *selTimer = timers[timer];

	// Agrego la funcion a ejecutar cuando el capture provoque una interrupcion
	TIMER_fun[timer][capture+4] = funcion;

	//Detectar capture en rising y habilito interrupciones
	//selTimer->CCR &= ~(1<<2);
	if (capture == 0)
	{
		selTimer->CCR |= (1 << 2);
		selTimer->CCR |= (1 << 1);
		selTimer->CCR &= ~(1 << 0);
	}
	else if (capture == 1)
	{
		selTimer->CCR |= (1 << 5);
		selTimer->CCR |= (1 << 4);
		selTimer->CCR &= ~(1 << 3);
	}

	//selTimer->CCR |= (1<< (capture*3)) | (1<< (2+capture*3));

	// Pongo todo a 0 porque no lo quiero como contador
	//ctcr[timer][0] &= ~(0xF);
}

/**
 * Analiza que interrupcion fue la que se lanzo y la resetea. Luego ejecuta la funcion asociada a esa interrupcion
 * @param intervalo nodo a ser analizado
 * @return void no retorna nada
 */
inline void analizarTimer(uint32_t timer){
	int i;
	timer_t *timerAnalizar = timers[timer];
	timerFun *funcionesTimer = TIMER_fun[timer];

	//Me fijo cual de las 6 interrupciones fue y la reseteo. Luego ejecuto la funcion asociada
	for(i=0; i<6;i++){
		if(timerAnalizar->IR & (1<<i)){
			timerAnalizar->IR = (1<<i);
			if(funcionesTimer[i] != 0)
				funcionesTimer[i](timerAnalizar);
		}
	}
}

void TIMER0_IRQHandler(){
	analizarTimer(0);
}

void TIMER1_IRQHandler(){
	analizarTimer(1);
}

void TIMER2_IRQHandler(){
	analizarTimer(2);
}

void TIMER3_IRQHandler(){
	analizarTimer(3);
}

//*************************************************************************************************************************************//
//***************************************************FIN Timers************************************************************************//
//*************************************************************************************************************************************//




//*************************************************************************************************************************************//
//***************************************************Ejemplos de Uso*******************************************************************//
//*************************************************************************************************************************************//
/*

|||||SysTick|||||

	void myFuncion(SysTickIntervalo *intervalo){
		...
	}

	InicSysTick();
	addIntervalo(&myFuncion,500);

	Ejecuta myFuncion cada medio segundo
	la funcion recive el nodo para poder editar cosas. Por ejemplo el tiempo

|||||SysTick|||||

|||||Match|||||

	#define miliToMicro(x) 1000*x

	int timer = 0; // Timer a usar

	// Las funciones reciven el timer
	void funcion0(timer_t *t){...}
	void funcion1(timer_t *t){...}
	void funcion2(timer_t *t){...}

	Inicializar_Match(timer,0,miliToMicro(1000),0, &funcion0); // Al segundo de iniciar el timer
	Inicializar_Match(timer,1,miliToMicro(0),0,&funcion1);	// Al mismo tiempo de inicar el timer
	Inicializar_Match(timer,2,miliToMicro(2500),0,&funcion2); // A los 2 segundos y medio de iniciar el timer
	Inicializar_Match(timer,3,miliToMicro(3000),1,0); // No ejecuta nada pero resetea el timer a los 3 segundos
	Inicializar_Timer(timer);


|||||Match|||||

|||||Capture|||||

	int timer = 0; // Timer a usar

	// Las funciones reciven el timer
	// Muestra el valor del timer counter y no reinicia
	void myFuncion(timer_t *t){
		printf("El valor es: %i\n",t->TC);
		t->TC = 0;
	}


	Inicializar_Capture(timer,0,&myFuncion);
	Inicializar_Timer(timer);


|||||Capture|||||

 */
