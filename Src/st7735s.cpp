/*
 * st7735s.cpp
 *
 *  Created on: Jan 8, 2023
 *      Author: marzdz
 */
#include "st7735s.h"
#include "stm32f103xb.h"
#include "fonts.h"

#define DELAY 0x80

// based on Adafruit ST7735 library for Arduino
//static const uint8_t
//  init_cmds1[] = {            // Init for 7735R, part 1 (red or green tab)
//    15,                       // 15 commands in list:
//    ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
//      150,                    //     150 ms delay
//    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
//      255,                    //     500 ms delay
//    ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
//      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
//    ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
//      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
//    ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
//      0x01, 0x2C, 0x2D,       //     Dot inversion mode
//      0x01, 0x2C, 0x2D,       //     Line inversion mode
//    ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
//      0x07,                   //     No inversion
//    ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
//      0xA2,
//	  //0x1C,						//	-3,3V
//      0x02,                   //     -4.6V
//      0x84,                   //     AUTO mode
//    ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
//      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
//    ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
//      0x0A,                   //     Opamp current small
//      0x00,                   //     Boost frequency
//    ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
//      0x8A,                   //     BCLK/2, Opamp current small & Medium low
//      0x2A,
//    ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
//      0x8A, 0xEE,
//    ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
//      0x0E,
//    ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
//    ST7735_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
//      ST7735_ROTATION,        //     row addr/col addr, bottom to top refresh
//    ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
//      0x05 },                 //     16-bit color
//
//
//  init_cmds2[] = {            // Init for 7735R, part 2 (1.44" display)
//    2,                        //  2 commands in list:
//    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
//      0x00, 0x00,             //     XSTART = 0
//      0x00, 0x7F,             //     XEND = 127
//    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
//      0x00, 0x00,             //     XSTART = 0
//      0x00, 0x7F },           //     XEND = 127
//
//  init_cmds3[] = {            // Init for 7735R, part 3 (red or green tab)
//    4,                        //  4 commands in list:
//    ST7735_GMCTRP1, 16      , //  1: Gamma Adjustments (pos. polarity), 16 args, no delay:
//      0x02, 0x1c, 0x07, 0x12,
//      0x37, 0x32, 0x29, 0x2d,
//      0x29, 0x25, 0x2B, 0x39,
//      0x00, 0x01, 0x03, 0x10,
//    ST7735_GMCTRN1, 16      , //  2: Gamma Adjustments (neg. polarity), 16 args, no delay:
//      0x03, 0x1d, 0x07, 0x06,
//      0x2E, 0x2C, 0x29, 0x2D,
//      0x2E, 0x2E, 0x37, 0x3F,
//      0x00, 0x00, 0x02, 0x10,
//    ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
//      10,                     //     10 ms delay
//    ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
//      100 };                  //     100 ms delay

static const uint8_t
  init_cmds1[] = {                        // Init commands for 7735B screens
    18,                             // 18 commands in list:
	ST7735_SWRESET,   DELAY, //  1: Software reset, no args, w/delay
      50,                           //     50 ms delay
	  ST7735_SLPOUT,    DELAY, //  2: Out of sleep mode, no args, w/delay
      255,                          //     255 = max (500 ms) delay
	  ST7735_COLMOD,  1, //  3: Set color mode, 1 arg + delay:
      0x05,   DELAY,                      //     16-bit color
      10,                           //     10 ms delay
    ST7735_FRMCTR1, 3, //  4: Frame rate control, 3 args + delay:
      0x00,                         //     fastest refresh
      0x06,                         //     6 lines front porch
      0x03,      DELAY,                   //     3 lines back porch
      10,                           //     10 ms delay
    ST7735_MADCTL,  1,              //  5: Mem access ctl (directions), 1 arg:
      0x08,                         //     Row/col addr, bottom-top refresh
    ST7735_DISSET5, 2,              //  6: Display settings #5, 2 args:
      0x15,                         //     1 clk cycle nonoverlap, 2 cycle gate
                                    //     rise, 3 cycle osc equalize
      0x02,                         //     Fix on VTL
    ST7735_INVCTR,  1,              //  7: Display inversion control, 1 arg:
      0x0,                          //     Line inversion
    ST7735_PWCTR1,  2, //  8: Power control, 2 args + delay:
      0x02,                         //     GVDD = 4.7V
      0x70,        DELAY,                 //     1.0uA
      10,                           //     10 ms delay
    ST7735_PWCTR2,  1,              //  9: Power control, 1 arg, no delay:
      0x05,                         //     VGH = 14.7V, VGL = -7.35V
    ST7735_PWCTR3,  2,              // 10: Power control, 2 args, no delay:
      //0x01,                         //     Opamp current small
		0x2D,						//OPamp large current
      0x02,                         //     Boost frequency
    ST7735_VMCTR1,  2, // 11: Power control, 2 args + delay:
      0x3C,                         //     VCOMH = 4V
      0x38,             DELAY,            //     VCOML = -1.1V
      10,                           //     10 ms delay
    ST7735_PWCTR6,  2,              // 12: Power control, 2 args, no delay:
      0x11, 0x15,
    ST7735_GMCTRP1,16,              // 13: Gamma Adjustments (pos. polarity), 16 args + delay:
      0x09, 0x16, 0x09, 0x20,       //     (Not entirely necessary, but provides
      0x21, 0x1B, 0x13, 0x19,       //      accurate colors)
      0x17, 0x15, 0x1E, 0x2B,
      0x04, 0x05, 0x02, 0x0E,
    ST7735_GMCTRN1,16, // 14: Gamma Adjustments (neg. polarity), 16 args + delay:
      0x0B, 0x14, 0x08, 0x1E,       //     (Not entirely necessary, but provides
      0x22, 0x1D, 0x18, 0x1E,       //      accurate colors)
      0x1B, 0x1A, 0x24, 0x2B,
      0x06, 0x06, 0x02, 0x0F,DELAY,
      10,                           //     10 ms delay
	  ST7735_CASET,   4,              // 15: Column addr set, 4 args, no delay:
      0x00, 0x02,                   //     XSTART = 2
      0x00, 0x81,                   //     XEND = 129
	  ST7735_RASET,   4,              // 16: Row addr set, 4 args, no delay:
      0x00, 0x02,                   //     XSTART = 1
      0x00, 0x81,                   //     XEND = 160
	  ST7735_NORON,     DELAY, // 17: Normal display on, no args, w/delay
      10,                           //     10 ms delay
	  ST7735_DISPON,    DELAY, // 18: Main screen turn on, no args, delay
      255 },                        //     255 = max (500 ms) delay

	init_cmds2[] = {            // 7735R init, part 2 (mini 160x80)
    2,                              //  2 commands in list:
	ST7735_CASET,   4,              //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,                   //     XSTART = 0
      0x00, 0x4F,                   //     XEND = 79
	  ST7735_RASET,   4,              //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,                   //     XSTART = 0
      0x00, 0x9F },                 //     XEND = 159

	init_cmds3[] = {                       // 7735R init, part 3 (red or green tab)
    4,                              //  4 commands in list:
    ST7735_GMCTRP1, 16      ,       //  1: Gamma Adjustments (pos. polarity), 16 args + delay:
      0x02, 0x1c, 0x07, 0x12,       //     (Not entirely necessary, but provides
      0x37, 0x32, 0x29, 0x2d,       //      accurate colors)
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      ,       //  2: Gamma Adjustments (neg. polarity), 16 args + delay:
      0x03, 0x1d, 0x07, 0x06,       //     (Not entirely necessary, but provides
      0x2E, 0x2C, 0x29, 0x2D,       //      accurate colors)
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
	  ST7735_NORON,     DELAY, //  3: Normal display on, no args, w/delay
      10,                           //     10 ms delay
	  ST7735_DISPON,    DELAY, //  4: Main screen turn on, no args w/delay
      100 };                        //     100 ms delay

ST7735S::ST7735S()
{
	spi_st7735s.setupSPI(SPI2);
	DC.setupGPIO(GPIOA, MODE::OUT_PUSH_PULL, SPEED::SP_10_MHZ, 12);
	RST.setupGPIO(GPIOA, MODE::OUT_PUSH_PULL, SPEED::SP_10_MHZ, 11);
	CS.setupGPIO(GPIOB, MODE::OUT_PUSH_PULL, SPEED::SP_10_MHZ, 12);
	RST.write();
}

void ST7735S::init()
{
	select();
	reset();
	DC.reset();
	executeCommandList(init_cmds1);
	executeCommandList(init_cmds2);
	executeCommandList(init_cmds3);
	unselect();
}

void ST7735S::select()
{
	CS.reset();
}

void ST7735S::unselect()
{
	CS.write();
}

void ST7735S::reset()
{
	RST.reset();
	//wait 5 ms
	TIM2->CNT = 0;
	while(TIM2->CNT < 5);				//wait 5ms; it's assumed that TIM2 is running. It needs more standardize
	RST.write();
}

void ST7735S::fillScreen(uint16_t color)
{
	m_x = 0;
	m_y = 0;
	m_width = ST7735_WIDTH;
	m_height = ST7735_HEIGHT;
	m_color = color;
	fillRectangle();
}

void ST7735S::fillRectangle()
{
	select();
	setAddressWindow();
	m_dataBuffer[0] = m_color >> 8;
	m_dataBuffer[1] = m_color & 0xFF;
	m_dataSize = 2;
	DC.write();
	for(uint8_t i = m_height; i > 0; i--)
	{
		for(uint8_t j = m_width; j > 0; j--)
		{
			spi_st7735s.dataTransmit(m_dataBuffer, m_dataSize);
		}
	}
	unselect();
}

void ST7735S::setAddressWindow()
{
	m_commandBuffer = ST7735_CASET;
	writeCommand();

	m_dataBuffer[0] = 0x00;
	m_dataBuffer[1] = m_x + ST7735_XSTART;
	m_dataBuffer[2] = 0x00;
	m_dataBuffer[3] = m_x + m_width - 1 + ST7735_XSTART;
	m_dataSize = 4;
	writeData();

	m_commandBuffer = ST7735_RASET;
	writeCommand();
	m_dataBuffer[1] = m_y + ST7735_YSTART;
	m_dataBuffer[3] = m_y + m_height - 1 + ST7735_YSTART;
	writeData();

	m_commandBuffer = ST7735_RAMWR;
	writeCommand();
}

void ST7735S::writeCommand()
{
	DC.reset();
	spi_st7735s.dataTransmit(&m_commandBuffer, 1);
}

void ST7735S::writeData()
{
	DC.write();
	spi_st7735s.dataTransmit(m_dataBuffer, m_dataSize);
}

void ST7735S::writeDatatmp(uint8_t* tab, uint8_t size)
{
	DC.write();
	spi_st7735s.dataTransmit(tab, size);
}

void ST7735S::executeCommandList(const uint8_t *addr)
{
	uint8_t numCommands, numArgs;
	uint16_t ms;

	numCommands = *addr++;
	while(numCommands--)
	{
		uint8_t cmd = *addr++;
		m_commandBuffer = cmd;
		writeCommand();

		numArgs = *addr++;
		// If high bit set, delay follows args
		ms = numArgs & DELAY;
		numArgs &= ~DELAY;
		if(numArgs) {
			writeDatatmp((uint8_t*)addr, numArgs);
			addr += numArgs;
		}

		if(ms) {
			ms = *addr++;
			if(ms == 255) ms = 500;
			TIM2->CNT = 0;
			while(TIM2->CNT < ms);
		}
	}
}

void ST7735S::printChar(uint8_t x, uint8_t y, unsigned char sign, uint16_t color, uint16_t bgColor)
{
	uint32_t i, b, j;

	m_x = x;
	m_y = y;
	m_width = m_x + Font_11x18.width;
	m_height = m_y + Font_11x18.height;
	setAddressWindow();						//ustawia gdzie beda zmienione piksele

	for(i = 0; i < Font_11x18.height; i++) {
		b = Font_11x18.data[(sign - 32) * Font_11x18.height + i];
		for(j = 0; j < Font_11x18.width; j++) {
			if((b << j) & 0x8000)  {
				m_dataBuffer[0] = color >> 8;
				m_dataBuffer[1] = color & 0xFF;
				m_dataSize = 2;
				writeData();
			} else {
				m_dataBuffer[0] = bgColor >> 8;
				m_dataBuffer[1] = bgColor & 0xFF;
				m_dataSize = 2;
				writeData();
			}
		}
	}
}

void ST7735S::writeString(uint8_t x, uint8_t y, const char* str, uint16_t color, uint16_t bgcolor)
{
	select();

	while(*str) {
		if(x + Font_11x18.width >= ST7735_WIDTH) {
			x = 0;
			y += Font_11x18.height;
			if(y + Font_11x18.height >= ST7735_HEIGHT) {
				break;
			}

			if(*str == ' ') {
				// skip spaces in the beginning of the new line
				str++;
				continue;
			}
		}

		printChar(x, y, *str, color, bgcolor);
		x += Font_11x18.width;
		str++;
	}

	unselect();
}
