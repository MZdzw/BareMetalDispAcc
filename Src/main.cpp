/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

//#include <stdint.h>
#include "rccCL.h"
#include "gpioCL.h"
#include "timCL.h"
#include "st7735s.h"

/*#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif*/
//uint8_t __attribute__(( section(".ram1sectionBss") )) ads_buf[5];
//uint8_t __attribute__(( section(".ram1sectionData") )) ads_buf_data[4] = { 1, 2, 3, 4 };

int main(void)
{
    /* Loop forever */
	Rcc rcc(IN_OUT_CLK::HSE, SYS_CLK_MUX::PLLCLK, RCC_CFGR_HPRE_DIV1, RCC_CFGR_PPRE1_DIV1,
			RCC_CFGR_PPRE2_DIV1, RCC_CFGR_PLLMULL4);

	pTIM tim2(TIM2, 0, 0xFFFF-1, 32000-1);

	pGPIO led(GPIOC, MODE::OUT_PUSH_PULL, SPEED::SP_2_MHZ, 13);

	ST7735S st7735s;
	st7735s.init();
	//uint8_t temp = 1;
	st7735s.fillScreen(ST7735_WHITE);

	TIM2->CNT = 0;
	for(;;)
	{
		if(tim2.getCounter() > 1000)		//each 5 ms
		{
			led.toggle();		//toggle 13 pin
//			temp ^= 0x03;
//			if(temp & 0x01)
//				st7735s.fillScreen(ST7735_BLACK);
//			else
//				st7735s.fillScreen(ST7735_YELLOW);
			st7735s.writeString(40, 20, "A", ST7735_BLACK, ST7735_WHITE);
			tim2.resetCounter();

		}
	}
}
