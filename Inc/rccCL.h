/*
 * rccCL.h
 *
 *  Created on: Nov 16, 2022
 *      Author: Marcin
 */

#ifndef RCCCL_H_
#define RCCCL_H_

#include "stm32f103xb.h"

enum class IN_OUT_CLK
{
	HSI, HSE
};

enum class SYS_CLK_MUX
{
	HSI, HSE, PLLCLK
};

class Rcc
{
public:
	Rcc() {};
	Rcc(IN_OUT_CLK, SYS_CLK_MUX, uint32_t, uint32_t, uint32_t, uint32_t);

	uint32_t get_PCLK2_clock_speed();
};

#endif /* RCCCL_H_ */
