// ------------ Display parameters -----------------------
// Overall display parameters
#define		ScreenX0 		0
#define		ScreenY0		0
//TODO dinamizar las resoluciones
#define		ScreenXsize		320
#define		ScreenYsize		240

#define		Rbits			0		// Red bits position
#define		Gbits			5		// Green bits position
#define		Bbits			11		// Blue bits position
#define		RGB(R, G, B)	(((R >> 3) << Rbits) | ((G >> 2) << Gbits) | ((B >> 3) << Bbits))

#define	 LCD_W  320
#define	 LCD_H  240


//define de los comandos
#define ILI9341_SOFTRESET          0x01
#define ILI9341_SLEEPIN            0x10
#define ILI9341_SLEEPOUT           0x11
#define ILI9341_NORMALDISP         0x13
#define ILI9341_INVERTOFF          0x20
#define ILI9341_INVERTON           0x21
#define ILI9341_GAMMASET           0x26
#define ILI9341_DISPLAYOFF         0x28
#define ILI9341_DISPLAYON          0x29
#define ILI9341_COLADDRSET         0x2A
#define ILI9341_PAGEADDRSET        0x2B
#define ILI9341_MEMORYWRITE        0x2C
#define ILI9341_PIXELFORMAT        0x3A
#define ILI9341_FRAMECONTROL       0xB1
#define ILI9341_DISPLAYFUNC        0xB6
#define ILI9341_ENTRYMODE          0xB7
#define ILI9341_POWERCONTROL1      0xC0
#define ILI9341_POWERCONTROL2      0xC1
#define ILI9341_VCOMCONTROL1       0xC5
#define ILI9341_VCOMCONTROL2       0xC7
#define ILI9341_MEMCONTROL         0x36
#define ILI9341_MADCTL		       0x36

#define ILI9341_MADCTL_MY  0x80
#define ILI9341_MADCTL_MX  0x40
#define ILI9341_MADCTL_MV  0x20
#define ILI9341_MADCTL_ML  0x10
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH 0x04


enum  COLOR{
	clBlack 			=	RGB(0, 0, 0),
	clWhite			= 	RGB(255, 255, 255),
	clRed			= 	RGB(255, 0, 0),
	clGreen			= 	RGB(0, 255, 0),
	clBlue			= 	RGB(0, 0, 255),
	clYellow			=	RGB(255, 255, 0),
	clGainsboro		=	RGB(220, 220, 220),
	clNavy			=	RGB(0, 0, 128),
	clAqua			=	RGB(0, 255, 255),
	clHotpink			=	RGB(255, 105, 180),
	clOrange			=	RGB(255, 165, 0),
	clDeepskyblue		=	RGB(0, 191, 255),
	clDimgray			=	RGB(105, 105, 105),
	cllightsalmon		=	RGB(255, 160, 122),
	cllightcoral		=	RGB(240, 128, 128),
	clpaleturquoise	=	RGB(175, 238, 238),
	clturquoise		=	RGB(64, 224, 208),
	clViolet			=	RGB(238, 130, 238),
	clGray1			=	RGB(90, 90, 90),
	clGray2			=	RGB(220, 220, 220),
	clGray3			=	RGB(240, 240, 240),
	clDarkgray		=	RGB(169, 169, 169),
	clSkyblue			= 	RGB(135, 206, 235),
	clChocolate		= 	RGB(210, 105, 30),
	clMediumseagreen	= 	RGB(60, 179, 113),
	clPeachpuff		=	RGB(255, 218, 185),
	clSeagreen		=	RGB(46, 139, 87),

	clBG1			= 	clGainsboro,

	clActiveItem1		=	clAqua,
	clActiveItem2		=	clturquoise,

	clBtnBG1			=	clOrange,
	clBtnBG2			= 	clBlue,
	clBtnBG3			=	clGainsboro,
	clBtnBG4			= 	clSkyblue,
	clBtnBG5			= 	clRed,

	clBtnFG1			=	clBlack,
	clBtnFG2			= 	clWhite,

	clCh1			= 	clYellow,
	clTB				=	clGreen,
	clTrigger			=	clHotpink,

	clCursorT			= 	clMediumseagreen,
	clCursorV			= 	clOrange,
	clCursorActive		=	clAqua,
	clMeasurement		= 	clGray3,
} ;

typedef struct {
	uint8_t 	*Array;
	uint8_t	Xsize;
	uint8_t	Ysize;
	uint8_t	CharPitch;
	uint8_t	LinePitch;
	uint8_t	IndexOfs;
	} FONT;

void SetWindow(uint16_t, uint16_t, uint16_t, uint16_t);
void escribirContinuo(uint16_t, uint16_t, uint16_t,uint16_t, uint16_t);
void PutsGenic(uint16_t, uint16_t, uint8_t*, uint16_t, uint16_t, FONT*);
void PutcGenic(uint16_t, uint16_t, uint8_t, uint16_t, uint16_t, FONT*);
void escribirDatoLCD(uint8_t);
void escribirComandoLCD(uint8_t);
void iniciarLCDILI();
void iniciarLCDSSD();
void DrawPixel(uint16_t, uint16_t, uint16_t);
void Delay_LCD(uint32_t Tiempo);
