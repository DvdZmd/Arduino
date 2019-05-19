#include "registros.h"

#define TIMER_DE_DELAY 0
#define UART_TRES 3

volatile extern uint16_t ultimaLectura;
extern timer_t* temporizador;
extern timer_t *timers[];
uint16_t muestras[320];
uint16_t buffer[LCD_W][LCD_H];

void iniciarADCs ()
{
	gpioConfig (0, 25, 1, PULLDOWN, SALIDA);
	inicilizarADC (BURST_OFF, ADCS_LECTURA_JOYSTICK, INHABILITAR_INTERRUPCION);
}
void intercambiar (uint32_t * A, int i, int j)
{
	uint32_t tmp = A[i];
	A[i] = A[j];
	A[j] = tmp;
}
void ordenacion_seleccion (uint32_t * A, int N)
{
	int i, j, k;
	for (i = 0; i < N - 1; i++)
	{
		for (k = i, j = i + 1; j < N; j++)
			if (A[j] < A[k])
				k = j;
		if (k != i)
			intercambiar (A, i, k);
	}
}

//Toma de forma bloqueante 10 lecturas del adc, las ordena y quita la primera y la última
//para eliminar los valores 0 y 1024, promedia las restantes y esa es la lectura que tomo como oficial
void DiezLecturas()
{
	static uint32_t buffPromedioADC[5]={0,0,0,0,0};//,0,0,0,0,0
	for(int i = 0; i<5;i++)
		buffPromedioADC[i] = primitivaBasica(ACD_CHANNEL_OSCILOSCOPOIO);
	
	ordenacion_seleccion(buffPromedioADC,5);

	int sumador = 0;
	for(int i = 1; i < 4 ; i++)
		sumador += buffPromedioADC[i];
	ultimaLectura = sumador / 3;

	uint8_t uno  = 0;
	uno = ultimaLectura >> 8;
	UART_Enviar (&uno, 1);

	uint8_t dos  = 0;
	dos = ultimaLectura;
	UART_Enviar (&dos, 1);

	uint8_t separador = 0;
	separador = 0xF0;
	UART_Enviar (&separador, 1);

	graficarSenial();

}
void initApp ()
{

	InitPLL ();
	iniciarSystick ();
	InicializarTimer (TIMER_DE_DELAY); //EL timer 0 es necesario por ser quien efectua los delays
	Init_UART (UART_TRES);

	iniciarADCs ();
	iniciarDisplay (ILI);

	delay (100);
	//
	//Estos dos barridos los hago como test del display
	escribirContinuo (ScreenX0, ScreenY0, LCD_W, LCD_H, clBlack);
	escribirContinuo (ScreenX0, ScreenY0, LCD_W, LCD_H, clWhite);
		
	InicializarMatch (1, 0, 1, 1, DiezLecturas);
	InicializarTimer (1);
	
	ejecutarFuncion (graficarSenial, 1, REPETIR);
}

void LCD_Borrar (void)
{
	for (int i = 0; i < 320; i++)
	
		muestras[i] = 0;
}
/**
 * Dibuja una representación del valor de tensión que está leyendo un canal específico del ADC
 * @param no recibe nada
 * @return void no retorna nada
 */
callback graficarSenial ()
{
	//La posición de X en mi display la representa el índice de mi vector de muestras = 320, el largo del display
	//La posición de Y es la representación del valor de tensión leido por el adc
	static uint8_t muestrasIndex = 0;
	int i = 0;
	int muestraAnterior = 0;

	if (ultimaLectura / 17 != 0)
		muestras[0] = ultimaLectura / 17;// 4096/17=240   --   240 es el ancho de pixeles donde voy a graficar
	else
		muestras[0] = 1;

	i = CANTIDAD_DE_MUESTRAS;

	for (i; i > 0; i--)
	{
		muestraAnterior = muestras[i];
		DrawPixel (i, muestraAnterior, clBlack);
		DrawPixel (i - 1, muestraAnterior, clWhite);
		muestras[i] = muestras[i - 1];
	}
}


