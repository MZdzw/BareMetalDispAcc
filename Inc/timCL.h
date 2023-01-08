/*
 * timCL.h
 *
 *  Created on: Nov 16, 2022
 *      Author: Marcin
 */

#ifndef TIMCL_H_
#define TIMCL_H_

#include "ioBaseCL.h"
#include "stm32f103xb.h"

class pTIM : public pIO
{
public:
	pTIM(TIM_TypeDef*, uint8_t, uint16_t, uint16_t);

	uint16_t getCounter() const;
	void resetCounter();
private:
	TIM_TypeDef* tim;
};

#endif /* TIMCL_H_ */
