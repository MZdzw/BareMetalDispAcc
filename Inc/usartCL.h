/*
 * usartCL.h
 *
 *  Created on: Nov 16, 2022
 *      Author: Marcin
 */

#ifndef USARTCL_H_
#define USARTCL_H_

#include "stm32f103xb.h"
#include "ioBaseCl.h"

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

	void uartSendChar(char sign);

private:


	USART_TypeDef* uart;
	uint32_t enable_uart_bit = 0;
};

#endif /* USARTCL_H_ */
