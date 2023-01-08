/*
 * st7735s.h
 *
 *  Created on: Jan 8, 2023
 *      Author: marzdz
 */
#ifndef ST7735S_H_
#define ST7735S_H_

#include "spiCL.h"
#include "gpioCL.h"

#define ST7735_MADCTL_MY  0x80
#define ST7735_MADCTL_MX  0x40
#define ST7735_MADCTL_MV  0x20
#define ST7735_MADCTL_ML  0x10
#define ST7735_MADCTL_RGB 0x00
#define ST7735_MADCTL_BGR 0x08
#define ST7735_MADCTL_MH  0x04

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_GAMSET  0x26
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_IS_160X128 1
#define ST7735_WIDTH  128
#define ST7735_HEIGHT 160
#define ST7735_XSTART 2
#define ST7735_YSTART 1
//default display
//#define ST7735_ROTATION (ST7735_MADCTL_MX | ST7735_MADCTL_MY | ST7735_MADCTL_RGB)
//upside down display
#define ST7735_ROTATION (ST7735_MADCTL_RGB)

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

// Color definitions
#define	ST7735_BLACK   0x0000
#define	ST7735_BLUE    0x001F
#define	ST7735_RED     0xF800
#define	ST7735_GREEN   0x07E0
#define ST7735_CYAN    0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0xFFE0
#define ST7735_WHITE   0xFFFF
#define ST7735_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

class ST7735S
{
private:
	pSPI spi_st7735s;
	pGPIO RST;
	pGPIO DC;
	pGPIO CS;

	uint8_t m_x;
	uint8_t m_y;
	uint8_t m_width;
	uint8_t m_height;

	//buffer for data
	uint8_t m_dataSize;
	uint8_t m_dataBuffer[4];	//32 bits
	//buffer for command
	uint8_t m_commandBuffer;
	uint16_t m_color;

	void select();
	void unselect();
	void reset();
	void fillRectangle();
	void setAddressWindow();
	void writeCommand();
	void writeData();
	void writeDatatmp(uint8_t*, uint8_t);
	void executeCommandList(const uint8_t *addr);
public:
	ST7735S();
	void init();
	void fillScreen(uint16_t);
	void printChar(uint8_t, uint8_t ,unsigned char, uint16_t, uint16_t);
	void writeString(uint8_t, uint8_t, const char*, uint16_t, uint16_t);
};



#endif /* ST7735S_H_ */
