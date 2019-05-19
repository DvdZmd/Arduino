#define TIMER_CERO ((uint32_t*) 0x40004000)

#define		TIMER0PW	1
#define		TIMER1PW	2
#define		TIMER2PW	22
#define		TIMER3PW	23


//Estructura para manejar los Timers:
typedef struct
{
	unsigned int	IR;
	unsigned int	TCR;
	unsigned int 	TC;
	unsigned int 	PR;
	unsigned int 	PC;
	unsigned int 	MCR;
	unsigned int	MR0;
	unsigned int	MR1;
	unsigned int	MR2;
	unsigned int	MR3;
	unsigned int	CCR;
	unsigned int	CR0;
	unsigned int	CR1;

} timer_t;


//Timers:
//0x40004000UL : Direccion de inicio de los registros del Timer0
#define		DIR_TIMER0		( ( timer_t* ) 0x40004000UL )
//0x40008000UL : Direccion de inicio de los registros del Timer1
#define		DIR_TIMER1		( ( timer_t* ) 0x40008000UL )
//0x40090000UL : Direccion de inicio de los registros del Timer2
#define		DIR_TIMER2		( ( timer_t* ) 0x40090000UL )
//0x40094000UL : Direccion de inicio de los registros del Timer3
#define		DIR_TIMER3		( ( timer_t* ) 0x40094000UL )


typedef void (*timerFun)(timer_t*);

void delay(uint32_t);
void analizarTimer(uint32_t timer);
void InicializarTimer(uint32_t);
void InicializarMatch(uint32_t, uint32_t, uint32_t, uint32_t, timerFun);

