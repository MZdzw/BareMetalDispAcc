/*
 * timCL.cpp
 *
 *  Created on: Nov 16, 2022
 *      Author: Marcin
 */
#include "timCL.h"

pTIM::pTIM(TIM_TypeDef* tim_arg,uint8_t mode, uint16_t auto_reload, uint16_t prescaler)
:tim(tim_arg)
{
	if(mode == 0)		//normal mode
	{
		//1. Enable Timer Clock
		if(tim == TIM2)
			RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

		//2. Set the prescaler and the ARR
		tim->ARR = 0xFFFF-1;
		tim->PSC = 32000-1;

		//3. Enable Timer
		tim->CR1 |= TIM_CR1_CEN;
		//Update interrupt flag.. This bit is set by hardware when the registers are updated
		while(!(tim->SR & TIM_SR_UIF));
	}
}

uint16_t pTIM::getCounter() const
{
	return tim->CNT;
}

void pTIM::resetCounter()
{
	tim->CNT = 0;
}



