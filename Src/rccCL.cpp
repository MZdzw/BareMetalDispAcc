/*
 * rccCL.cpp
 *
 *  Created on: Nov 16, 2022
 *      Author: Marcin
 */
#include "rccCL.h"

Rcc::Rcc(IN_OUT_CLK clk_src, SYS_CLK_MUX clk_sel, uint32_t hclk_psc, uint32_t pclk1_psc,
		uint32_t pclk2_psc, uint32_t pll_mul)
{
	if(clk_src == IN_OUT_CLK::HSE)
	{
	//1. ENABLE HSE and for the HSE to become Ready
		RCC->CR |= RCC_CR_HSEON;
		while(!(RCC->CR & RCC_CR_HSERDY));
	}
	else
	{
		RCC->CR |= RCC_CR_HSION;
		while(!(RCC->CR & RCC_CR_HSIRDY));
	}

	//2. Set the POWER ENABLE CLOCK
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

	//3. Configure the FLASH PREFATCH and the LATENCY Related Setting
	FLASH->ACR |= FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_1;

	//4a. PLL entry clock source
	(clk_src == IN_OUT_CLK::HSE) ? RCC->CFGR |= RCC_CFGR_PLLSRC : RCC->CFGR &= ~RCC_CFGR_PLLSRC;

	//4. Configure the PRESCALERS HCLK, PCLK1, PCLK2
	//AHP Prescaler
	RCC->CFGR |= hclk_psc;
	//APB1 Prescaler
	RCC->CFGR |= pclk1_psc;
	//APB2 Prescaler
	RCC->CFGR |= pclk2_psc;

	//5. Configure PLL
	RCC->CFGR |= pll_mul;

	//6. Enable PLL and wait for READY
	if(clk_sel == SYS_CLK_MUX::PLLCLK)
	{
		RCC->CR |= RCC_CR_PLLON;
		while(!(RCC->CR & RCC_CR_PLLRDY));
	}

	//7. Select the clock source
	if(clk_sel == SYS_CLK_MUX::PLLCLK)
	{
		RCC->CFGR |= RCC_CFGR_SW_PLL;
		while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
	}
	else if(clk_sel == SYS_CLK_MUX::HSE)
	{
		RCC->CFGR |= RCC_CFGR_SW_HSE;
		while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE);
	}
	else if(clk_sel == SYS_CLK_MUX::HSE)
	{
		RCC->CFGR |= RCC_CFGR_SW_HSI;
		while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);
	}
}

uint32_t Rcc::get_PCLK2_clock_speed()
{
	uint8_t used_clock = (RCC->CFGR & RCC_CFGR_SWS) >> RCC_CFGR_SWS_Pos;
	//0-HSI, 1-HSE, 2-PLL
	uint8_t pll_mul = (RCC->CFGR & RCC_CFGR_PLLMULL) >> RCC_CFGR_PLLMULL_Pos;
	if(pll_mul != 0x0D)
		pll_mul += 2;
	uint16_t hclk_psc = (RCC->CFGR & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos;
	if(hclk_psc == 0x00)
		hclk_psc = 1;
	else if(hclk_psc == 0x08)
		hclk_psc = 2;
	else if(hclk_psc == 0x0009)
		hclk_psc = 4;
	else if(hclk_psc == 0x000A)
		hclk_psc = 8;
	else if(hclk_psc == 0x000B)
		hclk_psc = 16;
	else if(hclk_psc == 0x000C)
		hclk_psc = 64;
	else if(hclk_psc == 0x000D)
		hclk_psc = 128;
	else if(hclk_psc == 0x000E)
		hclk_psc = 256;
	else if(hclk_psc == 0x000F)
		hclk_psc = 512;

	uint8_t pclk2_psc = (RCC->CFGR & RCC_CFGR_PPRE2) >> RCC_CFGR_HPRE_Pos;
	if(pclk2_psc == 0x00)
		pclk2_psc = 1;
	else if(pclk2_psc == 0x04)
		pclk2_psc = 2;
	else if(pclk2_psc == 0x05)
		pclk2_psc = 4;
	else if(pclk2_psc == 0x06)
		pclk2_psc = 8;
	else if(pclk2_psc == 0x07)
		pclk2_psc = 16;

	if(used_clock == 0x02)			//PLL
		return 8000000*pll_mul/hclk_psc/pclk2_psc;
	else if(used_clock == 0x01)		//HSE
		return 8000000/hclk_psc/pclk2_psc;
	else if(used_clock == 0x00)		//HSE
		return 8000000/hclk_psc/pclk2_psc;
	else
		return 0;
}




