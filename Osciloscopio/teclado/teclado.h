#define CANTIDAD_ENTRADAS 6
#define CANTIDAD_ESTABLE 5

typedef struct structEntradas{
	unsigned char estadoAnterior;
	unsigned char estadoActual;
	unsigned char contador;
	unsigned char estadoFiltrado;
}structEntradas;


uint8_t tecladoBarrido(void);
void tecladoAntirebote(SysTickIntervalo *);
uint8_t leerTeclado(void);
