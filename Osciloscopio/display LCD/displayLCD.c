#include "registros.h"

void
iniciarDisplay (uint8_t disp)
{
	IniciarBusParalelo (BUS_PARALELO);

	if (disp == ILI)
		iniciarLCDILI ();
	else if (disp = SSD)
		IniciarLCDSSD ();

	LCD_Borrar();

}


/**
 * Setea en el display el area de memoria que se va a escribir, pero no abré la escritura
 * @param x posición x en el display del punto inicial del area
 * @param y posición y en el display del punto inicial del area
 * @param x posición xsize en el display del punto final del area
 * @param y posición ysize en el display del punto final del area
 * @return void no retorna nada
 */
void setArea(uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize) {
	escribirComandoLCD(0x2A);

	escribirDatoLCD(x >> 8);
	escribirDatoLCD(x);
	escribirDatoLCD(xsize >> 8);
	escribirDatoLCD(xsize);

	escribirComandoLCD(0x2B);
	escribirDatoLCD(y >> 8);
	escribirDatoLCD(y);
	escribirDatoLCD(ysize >> 8);
	escribirDatoLCD(ysize);

	escribirComandoLCD(0x2C); //memory write -- Siguiente paso es escribír los pixeles

}

/**
 * Escribe el color en el pixel en foco
 * @return void no retorna nada
 */
void setColor(uint16_t colour) {
	escribirDatoLCD(colour >> 8);
	escribirDatoLCD(colour);
}

/**
 * Pinta un pixel con el color pasado verificando que no exeda los límites de resolución
 * @param x posición x en el display
 * @param y posición y en el display
 * @param color color elegido
 * @return void no retorna nada
 */
void DrawPixel(uint16_t x, uint16_t y, uint16_t color) //pixels will always be counted from right side.x is representing LCD width which will always be less tha 240.Y is representing LCD height which will always be less than 320
{
	if ((x < 0) || (x > LCD_W) || (y < 0) || (y > LCD_H)) //Si recibo valores mayores a los de la resolución, no hago nada
		return;

	escribirComandoLCD(0x2A);//Column Address Set
	escribirDatoLCD(x >> 8);
	escribirDatoLCD(x);

	escribirComandoLCD(0x2B);// Page Address Set
	escribirDatoLCD(y >> 8);
	escribirDatoLCD(y);

	escribirComandoLCD(0x2C); //memory write

	setColor(color);
}


void graficarRectaV(uint16_t x, uint16_t y,uint16_t l,uint16_t color)
{
	static int tmp = 0;
	escribirComandoLCD(0x2A);

	escribirDatoLCD(x >> 8);
	escribirDatoLCD(x);
	escribirDatoLCD(x >> 8);
	escribirDatoLCD(x);

	escribirComandoLCD(0x2B);
	escribirDatoLCD(y >> 8);
	escribirDatoLCD(y);
	escribirDatoLCD(l >> 8);
	escribirDatoLCD(l);

	escribirComandoLCD(0x2C); //memory write

	tmp = l;
	while (tmp) {
		setColor(color);
		tmp--;
	}
}


/**
 * Escribe color en en los pixeles de un area designada por un punto inicial y su largo en x e y
 * @param x posición x en el display
 * @param y posición y en el display
 * @param xsize largo a partir de la posición x en el display
 * @param ysize largo a partir de la  posición y en el display
 * @param color color elegido
 * @return void no retorna nada
 */
void escribirContinuo(uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize,
		uint16_t color) {
	uint32_t tmp;

	setArea(x, y, xsize, ysize);
	tmp = (uint32_t) xsize * (uint32_t) ysize;

	while (tmp) {
		setColor(color);
		tmp--;
	}
}

// Clear screen
void ClrScreen(void) {
	uint32_t tmp;

	escribirBarrido(ScreenX0, ScreenY0, ScreenXsize, ScreenYsize);
	tmp = (uint32_t) (ScreenXsize) * (uint32_t) (ScreenYsize);

	while (tmp) {
		setColor(clBlack);
		tmp--;
	}
}

// Put at (x, y) a 15X16 character [ch] with [f_color] and [b_color]
//
void PutcGenic(uint16_t x, uint16_t y, uint8_t ch, uint16_t fgcolor,
		uint16_t bgcolor, FONT* font) {
	uint8_t tmp, tmp2;
	uint16_t tmp1, tmp3;
	uint8_t *ptmp;

// Font address
	ptmp = (uint8_t *) font->Array
			+ (ch - font->IndexOfs) * ((font->Xsize + 7) / 8) * font->Ysize;

	SetWindow(x, font->Xsize, y, font->Ysize);

	gpioSet(RD, HIGH);
	gpioSet(RS, LOW);	//Va a escribir comando
	gpioSet(WR, LOW);
	escribirBusParalelo(BUS_PARALELO, 0x2C);
	
	gpioSet(WR, HIGH);	//Leo el bus paralelo en el LCD
	gpioSet(RS, HIGH);	//Termino de enviar comando

	tmp1 = (font->Xsize * font->Ysize) / 8;
	while (tmp1) {
		tmp = *ptmp;
		tmp2 = 8;
		while (tmp2) {
			tmp3 = (tmp & 0x80) ? fgcolor : bgcolor;
			gpioSet(WR, LOW);
			escribirBusParalelo(BUS_PARALELO, (tmp3 >> 8));
			gpioSet(WR, HIGH);
			gpioSet(WR, 0);
			escribirBusParalelo(BUS_PARALELO, tmp3);
			gpioSet(WR, 1);

			tmp <<= 1;
			tmp2--;
		}
		ptmp++;
		tmp1--;
	}

}

void PutsGenic(uint16_t x, uint16_t y, uint8_t *str, uint16_t fgcolor,
		uint16_t bgcolor, FONT* font) {
	uint8_t tmp;

	while ((tmp = *str++)) {
		PutcGenic(x, y, tmp, fgcolor, bgcolor, font);
		x += font->CharPitch;
		if (x >= ScreenXsize) {
			y += font->LinePitch;
			x = 0;
		}
	}
}

void escribirDatoLCD(uint8_t comando) {
	//Porque voy a escribir dato
	gpioSet(RS, HIGH);	//Termino de enviar comandos

	*FIO2PIN0 &= ~(0xFF);
	*FIO2PIN0 |= comando;

	gpioSet(WR, LOW);
	escribirBusParalelo(BUS_PARALELO, comando);

	Delay_LCD(UN_MICROSEGUNDO);

	gpioSet(WR, HIGH);
}

void escribirComandoLCD(uint8_t comando) {
	//gpioSet(RD, HIGH);
	gpioSet(RS, LOW);	//RS porque voy a escribir un comando

	escribirBusParalelo(BUS_PARALELO, comando);	//Escribo los pined
	*FIO2PIN0 &= ~(0xFF);
	*FIO2PIN0 |= comando;

	gpioSet(WR, LOW);	//Preparo el write para luego hacer un rising edge

	Delay_LCD(UN_MICROSEGUNDO);

	gpioSet(WR, HIGH);//El rising edge produce la lectura del LCD en el bus paralelo
	gpioSet(RS, HIGH);	//Termino de enviar comandos
}

void iniciarLCDILI() {
	uint8_t tmp;
	gpioSet(RESX, LOW);
	delay(1);
	gpioSet(RESX, HIGH);

	gpioSet(RESX, LOW);
	delay(15);
	gpioSet(RESX, HIGH);

	delay(200);

	//
	//	//memory access control
	//	//Data importante - Acá configuro datos del RGB y orientación del display
	//	escribirComandoLCD(ILI9341_MEMCONTROL);
	//	//escribirDatoLCD(0x48);//Cambia la orientación
	//	escribirDatoLCD(0x20);
	//
	//	//pixel format
	//	escribirComandoLCD(ILI9341_PIXELFORMAT);
	//	escribirDatoLCD(0x55);
	//
	//	//frameration control,normal mode full colours
	//	//ILI9341 pag 155 -- Información importante
	//	escribirComandoLCD(ILI9341_FRAMECONTROL);
	//	escribirDatoLCD(0x00);
	//	escribirDatoLCD(0x18);
	//
	//	//display function control
	//	//Mas data importante
	//	escribirComandoLCD(ILI9341_DISPLAYFUNC);
	//	//escribirDatoLCD(0x08);
	//	//escribirDatoLCD(0x82);
	//	//escribirDatoLCD(0x27);
	//	//write_comm(0xB6); // Display Function Control
	//	escribirDatoLCD(0x0a);
	//	escribirDatoLCD(0xE2);
	//
	//
	//Start initial Sequence

	delay(UN_MILISEGUNDO);

	escribirComandoLCD(0xcf);
	escribirDatoLCD(0x00);
	escribirDatoLCD(0xC1);
	escribirDatoLCD(0x30);

	escribirComandoLCD(0xed);
	escribirDatoLCD(0x67);
	escribirDatoLCD(0x03);
	escribirDatoLCD(0x12);
	escribirDatoLCD(0x81);

	escribirComandoLCD(0xcb);
	escribirDatoLCD(0x39);
	escribirDatoLCD(0x2c);
	escribirDatoLCD(0x00);
	escribirDatoLCD(0x34);
	escribirDatoLCD(0x02);

	escribirComandoLCD(0xea);
	escribirDatoLCD(0x00);
	escribirDatoLCD(0x00);

	escribirComandoLCD(0xe8);
	escribirDatoLCD(0x85);
	escribirDatoLCD(0x0a);
	escribirDatoLCD(0x78);

	escribirComandoLCD(0xF7);
	escribirDatoLCD(0x20);

	escribirComandoLCD(0xC0); //Power control
	escribirDatoLCD(0x26); //VRH[5:0]

	escribirComandoLCD(0xC1); //Power control
	escribirDatoLCD(0x01); //SAP[2:0];BT[3:0]

	escribirComandoLCD(0xC5); //VCM control
	escribirDatoLCD(0x2b);
	escribirDatoLCD(0x2F);

	escribirComandoLCD(0xc7);
	escribirDatoLCD(0xc7);

	escribirComandoLCD(0x3A);
	escribirDatoLCD(0x55);

	escribirComandoLCD(0x36); // Memory Access Control
//	escribirDatoLCD(0x08);
	escribirDatoLCD(0x20);

	escribirComandoLCD(0xB1); // Frame Rate Control
	escribirDatoLCD(0x00);
	escribirDatoLCD(0x18);

	escribirComandoLCD(0xB6); // Display Function Control
	escribirDatoLCD(0x0a);
	escribirDatoLCD(0xE2);

	escribirComandoLCD(0xF2); // 3Gamma Function Disable
	escribirDatoLCD(0x00);
	escribirComandoLCD(0x26); //Gamma curve selected
	escribirDatoLCD(0x01);
	escribirComandoLCD(0xE0); //Set Gamma
	escribirDatoLCD(0x0f);
	escribirDatoLCD(0x1d);
	escribirDatoLCD(0x1a);
	escribirDatoLCD(0x09);
	escribirDatoLCD(0x0f);
	escribirDatoLCD(0x09);
	escribirDatoLCD(0x46);
	escribirDatoLCD(0x88);
	escribirDatoLCD(0x39);
	escribirDatoLCD(0x05);
	escribirDatoLCD(0x0f);
	escribirDatoLCD(0x03);
	escribirDatoLCD(0x07);
	escribirDatoLCD(0x05);
	escribirDatoLCD(0x00);

	escribirComandoLCD(0XE1); //Set Gamma
	escribirDatoLCD(0x00);
	escribirDatoLCD(0x22);
	escribirDatoLCD(0x25);
	escribirDatoLCD(0x06);
	escribirDatoLCD(0x10);
	escribirDatoLCD(0x06);
	escribirDatoLCD(0x39);
	escribirDatoLCD(0x22);
	escribirDatoLCD(0x4a);
	escribirDatoLCD(0x0a);
	escribirDatoLCD(0x10);
	escribirDatoLCD(0x0c);
	escribirDatoLCD(0x38);
	escribirDatoLCD(0x3a);
	escribirDatoLCD(0x0F);

	escribirComandoLCD(0x11); //Exit Sleep
	delay(UN_MILISEGUNDO);

	escribirComandoLCD(0x29); //display on
}
void IniciarLCDSSD() {

	uint8_t tmp;
	gpioSet(RESX, LOW);
	delay(1);
	gpioSet(RESX, HIGH);

	gpioSet(RESX, LOW);
	delay(15);
	gpioSet(RESX, HIGH);

	delay(200);

	escribirComandoLCD(0xE2);		//PLL multiplier, set PLL clock to 120M
	escribirDatoLCD(0x1E);	    //N=0x36 for 6.5M, 0x23 for 10M crystal
	escribirDatoLCD(0x02);
	escribirDatoLCD(0x54);
	escribirComandoLCD(0xE0);		// PLL enable
	escribirDatoLCD(0x01);
	delay(100);
	escribirComandoLCD(0xE0);
	escribirDatoLCD(0x03);
	delay(100);
	escribirComandoLCD(0x01);		// software reset
	delay(100);
	escribirComandoLCD(0xE6);	//PLL setting for PCLK, depends on resolution
	escribirDatoLCD(0x03);
	escribirDatoLCD(0xFF);
	escribirDatoLCD(0xFF);

	escribirComandoLCD(0xB0);		//LCD SPECIFICATION
	escribirDatoLCD(0x04);
	escribirDatoLCD(0x00);
	escribirDatoLCD(0x03);		//Set HDP	799
	escribirDatoLCD(0x1F);
	escribirDatoLCD(0x01);		//Set VDP	479
	escribirDatoLCD(0xDF);
	escribirDatoLCD(0x00);

	escribirComandoLCD(0xB4);		//HSYNC
	escribirDatoLCD(0x03);		//Set HT	928
	escribirDatoLCD(0xA0);
	escribirDatoLCD(0x00);		//Set HPS	46
	escribirDatoLCD(0x2E);
	escribirDatoLCD(0x30);		//Set HPW	48
	escribirDatoLCD(0x00);		//Set LPS	15
	escribirDatoLCD(0x0F);
	escribirDatoLCD(0x00);

	escribirComandoLCD(0xB6);		//VSYNC
	escribirDatoLCD(0x02);		//Set VT	525
	escribirDatoLCD(0x0D);
	escribirDatoLCD(0x00);		//Set VPS	16
	escribirDatoLCD(0x10);
	escribirDatoLCD(0x10);		//Set VPW	16
	escribirDatoLCD(0x00);		//Set FPS	8
	escribirDatoLCD(0x08);

	escribirComandoLCD(0xBA);
	escribirDatoLCD(0x0F);		//GPIO[3:0] out 1

	escribirComandoLCD(0xB8);
	escribirDatoLCD(0x07);	    //GPIO3=input, GPIO[2:0]=output
	escribirDatoLCD(0x01);		//GPIO0 normal

	escribirComandoLCD(0x36);		//rotation
	escribirDatoLCD(0x22);

	escribirComandoLCD(0xF0);		//pixel data interface
	escribirDatoLCD(0x00);

	delay(1);

	setArea(0, 0, 799, 479);
	escribirComandoLCD(0x29);		//display on

	escribirComandoLCD(0xBE);		//set PWM for B/L
	escribirDatoLCD(0x06);
	escribirDatoLCD(0xf0);
	escribirDatoLCD(0x01);
	escribirDatoLCD(0xf0);
	escribirDatoLCD(0x00);
	escribirDatoLCD(0x00);

	escribirComandoLCD(0xd0);
	escribirDatoLCD(0x0d);

	escribirComandoLCD(0x2C);
	delay(1000 * 500);
}

void Delay_LCD(uint32_t Tiempo)
{
	do
	{
		Tiempo--;
	}while(Tiempo > 0);
}
