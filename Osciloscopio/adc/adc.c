#include "registros.h"

uint32_t* AdcsDataChannels[] =
		{ ADC0CD, ADC1CD, ADC2CD, ADC3CD, ADC4CD, ADC5CD,
				ADC6CD, ADC7CD };

volatile uint16_t ultimaLectura;
volatile uint8_t currentADCindex = 0;
uint8_t ADC_Iniciado = 0; //flag para no repetir procesos innecesariosal iniciar un adc mas de una vez

extern timer_t *timers[];


//Trigea y lee los adcs activos uno tras otro según aumenta currentADCindex
//Funcion pensada para usarse con Timers
callback triggerADCs ()
{
	dispararCanalADC (currentADCindex);
	currentADCindex = (currentADCindex + 1) % 2;
	ejecutarFuncion (readADCs, 1, NO_REPETIR);
}

//Lee todos los adcs activos uno detras del otro
//se debe llamar desde la funcion triggerADCs()
callback readADCs ()
{
	uint32_t* dataChannel = AdcsDataChannels[currentADCindex];
	
	//(*dataChannel >> 4) & 0x0FFF retorna el valor de la lectura del adc(dataChannel)
	if (currentADCindex == 0)
	{
		adcY = (*dataChannel >> 4) & 0x0FFF;
	}
	else
	{
		adcX = (*dataChannel >> 4) & 0x0FFF;
	}
}

callback triggerADC_Osciloscopio ()
{
	dispararCanalADC (ACD_CHANNEL_OSCILOSCOPOIO);
	ejecutarFuncion (readADC_OSCILOSCOIO, 5, NO_REPETIR);
}


callback readADC_OSCILOSCOIO ()
{
	uint32_t* dataChannel = AdcsDataChannels[ACD_CHANNEL_OSCILOSCOPOIO];
	
	//WARNING - De esta manera se queda el micro bloqueado hasta finalizar la convesión.
	//Para evitar esto usar triggerADCs()
	while (((*dataChannel >> 31) & 1) == 0)
	{
		dataChannel = AdcsDataChannels[ACD_CHANNEL_OSCILOSCOPOIO];
	}
	ultimaLectura = (*dataChannel >> 4) & 0x0FFF;//(*dataChannel >> 4) & 0x0FFF retorna el valor de la lectura del adc(dataChannel)
}

void inicilizarADC (uint8_t burstOn, uint8_t canales, uint8_t habilitarInterrupcion)
{

	if (ADC_Iniciado == 0)
	{
		*PCONP |= (1 << 12);
		PCLKSEL0&= ~(1 << 24);
		PCLKSEL0|= (1 << 25); // PCLKSEL_ADC = 100MHZ/2

		*ADCR |= (1 << 21);	// PDN = 1 - The A/D converter is operational
		*ADCR |= (64 << 8);	//Estos valores son para jugar segun frec
		ADC_Iniciado = 1;
	}
	*ADCR &= ~(0xFF);
	if (burstOn == 1)
	{
		*ADCR &= ~(0xFF);//dejo en 0 los bits que definen que canal voy a disparar
		*ADCR |= (canales);
		*ADCR |= (1 << 16);	//Activo BURST - los canales activados están constantemente convirtiendo y se pulea la obtención de resultados
		*ADCR &= ~(7 << 24);//Limpio estos tres bits para la configuracion del BURST. Lo pide la UM10360
	}
	else
		*ADCR &= ~(1 << 16);//BURST = 0, la ejecución del adc se controla con los bits 24 25 y 26

	if (habilitarInterrupcion == 1)
		*ISER0 |= (1 << 22);	// Habilito interrupcion ADC
	else
		*ISER0 &= ~(1 << 22);	// Inhabilito interrupcion ADC

}

void trigger (timer_t *t)
{
	dispararCanalADC (ACD_CHANNEL_OSCILOSCOPOIO);
}

callback dispararCanalADC (uint8_t canal)
{
	*ADCR &= ~(0xFF);//dejo en 0 los bits que definen que canal voy a disparar
	*ADCR |= (1 << canal);

	*ADCR &= ~(7 << 24);
	*ADCR |= (1 << 24);	//start conversion now. Esto se usa sin BUSRT
}

void ADC_IRQHandler ()
{ //Se llama cuando termina la conversión
}

uint32_t primitivaBasica (uint8_t canal)
{
	*ADCR &= ~(0xFF);//dejo en 0 los bits que definen que canal voy a disparar
	*ADCR |= (1 << canal);

	*ADCR &= ~(7 << 24);
	*ADCR |= (1 << 24);	//start conversion now. Esto se usa sin BUSRT

	uint32_t* dataChannel = AdcsDataChannels[canal];
	//WARNING - De esta manera se queda el micro bloqueado hasta finalizar la convesión.
	//Para evitar esto usar triggerADCs()
	while (((*dataChannel >> 31) & 1) == 0)
	{
		dataChannel = AdcsDataChannels[canal];
	}
	return ((*dataChannel >> 4) & 0x0FFF);//La expresion (*dataChannel >> 4) & 0x0FFF me devuelve el valor de la lectura del adc

}



