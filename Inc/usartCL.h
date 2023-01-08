/*
 * usartCL.h
 *
 *  Created on: Nov 16, 2022
 *      Author: Marcin
 */

#ifndef USARTCL_H_
#define USARTCL_H_

#include "stm32f103xb.h"
#include "ioBaseCL.h"
#include "interruptCL.h"

static InterruptHandler g_usart1_handler;

#ifdef __cplusplus
extern "C" {
#endif

void USART1_IRQHandler();

#ifdef __cplusplus
}
#endif

class pUART : public pIO
{
public:
	pUART(USART_TypeDef*);
	pUART(USART_TypeDef*, uint32_t);

	void clock_en(void) override;

	void send16BitNumber(uint16_t);
	void sendChar(char sign);

private:
	void isr();

	USART_TypeDef* uart;
	uint32_t enable_uart_bit = 0;
};

#endif /* USARTCL_H_ */
