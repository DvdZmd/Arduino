
#define		BURST_ON	1
#define		BURST_OFF	0

#define  ADCR     ((uint32_t*) 0x40034000UL)
#define  AD0GDR   ((uint32_t*) 0x40034004UL
#define  AD0INTEN ((uint32_t*) 0x4003400CUL)


#define ADC0CD ((uint32_t*) 0x40034010UL)
#define ADC1CD ((uint32_t*) 0x40034014UL)
#define ADC2CD ((uint32_t*) 0x40034018UL)
#define ADC3CD ((uint32_t*) 0x4003401CUL)
#define ADC4CD ((uint32_t*) 0x40034020UL)
#define ADC5CD ((uint32_t*) 0x40034024UL)
#define ADC6CD ((uint32_t*) 0x40034028UL)
#define ADC7CD ((uint32_t*) 0x4003402CUL)

#define ADSTAT ((uint32_t*) 0x40034030UL)

#define ACD_CHANNEL_OSCILOSCOPOIO  2

#define CANTIDAD_DE_MUESTRAS 320

void inicilizarADC(uint8_t , uint8_t, uint8_t );
void ADC_IRQHandler();
uint32_t primitivaBasica(uint8_t);
callback triggerADCs();
callback readADCs();
callback triggerADC_Osciloscopio();
callback readADC_OSCILOSCOIO();
void trigger (timer_t *t);
void lectura(timer_t *t);
callback dispararCanalADC(uint8_t);
