/*
 * usartCL.cpp
 *
 *  Created on: Nov 16, 2022
 *      Author: Marcin
 */
#include "usartCL.h"
#include "gpioCL.h"
#include "rccCL.h"

pUART::pUART(USART_TypeDef* uart_arg)
:uart(uart_arg)
{
	__disable_irq();
	enable_reg = &(RCC->APB2ENR);
	enable_bit = RCC_APB2ENR_IOPAEN;
	enable_uart_bit = RCC_APB2ENR_USART1EN;
	status = pSTATUS_ON;
	//PA10 - TX, PA9 - RX
	//RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	//enable USART1
	//RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	clock_en();

	pGPIO TX(GPIOA, MODE::AF_PUSH_PULL, SPEED::SP_2_MHZ, 9);
	pGPIO RX(GPIOA, MODE::AF_PUSH_PULL, SPEED::SP_2_MHZ, 10);

	//Setup the baudrate
	uart->BRR = 0x116;		//baudrate~115200 mantissa = 17, fraction = 6	= 17,375

	//enable UART transmit
	uart->CR1 |= USART_CR1_TE;

	//enable UART receive
	uart->CR1 |= USART_CR1_RE;

	//enable UART
	uart->CR1 |= USART_CR1_UE;

	//enable UART RX interrupt
	uart->CR1 |= USART_CR1_RXNEIE;

	if(uart == USART1)
		NVIC_EnableIRQ(USART1_IRQn);

	__enable_irq();
}

pUART::pUART(USART_TypeDef* uart_arg, uint32_t baudrate)
:uart(uart_arg)
{
	__disable_irq();
	enable_reg = &(RCC->APB2ENR);
	enable_bit = RCC_APB2ENR_IOPAEN;
	enable_uart_bit = RCC_APB2ENR_USART1EN;
	status = pSTATUS_ON;

	clock_en();

	if(uart == USART1)
	{
		pGPIO TX(GPIOA, MODE::AF_PUSH_PULL, SPEED::SP_2_MHZ, 9);
		pGPIO RX(GPIOA, MODE::AF_PUSH_PULL, SPEED::SP_2_MHZ, 10);
	}
	//Setup the baudrate
	Rcc pclk2_clk;
	float usart_div = static_cast<float>(pclk2_clk.get_PCLK2_clock_speed())/(16 * baudrate);
	uint8_t mantissa = usart_div;
	float fractionF = (usart_div - mantissa) * 16.0f;
	uint8_t fraction = fractionF;
	if(fractionF - static_cast<uint8_t>(fractionF) >= 0.5)
		fraction++;


	uart->BRR = (mantissa << 4) | (fraction << 0);

	//enable UART transmit
	uart->CR1 |= USART_CR1_TE;

	//enable UART receive
	uart->CR1 |= USART_CR1_RE;

	//enable UART
	uart->CR1 |= USART_CR1_UE;

	//enable UART RX interrupt
	uart->CR1 |= USART_CR1_RXNEIE;

	if(uart == USART1)
		NVIC_EnableIRQ(USART1_IRQn);

	__enable_irq();
	g_usart1_handler.connect<&pUART::isr>(this);
}

void pUART::clock_en(void)
{
	if (status == pSTATUS_ERR) { return; }
	*enable_reg |= enable_bit;
	*enable_reg |= enable_uart_bit;
	status = pSTATUS_ON;
}

void pUART::send16BitNumber(uint16_t num)
{
	char sign = num >> 8;
	sendChar(sign);
	sign = num & 0x00FF;
	sendChar(sign);
}

void pUART::sendChar(char sign)
{
	while(!(uart->SR & USART_SR_TXE));
	uart->DR = sign;
}

void pUART::isr()
{
	if(uart->SR & USART_SR_RXNE)
	{
		//clear RXNE bit in SR register if you dont read from DR register, otherwise you can but it is not obligatory
		char sign;
		sign = uart->DR;
		if(sign == 'D')
			GPIOC->ODR ^= GPIO_ODR_ODR13;
	}
}

void USART1_IRQHandler()
{
	g_usart1_handler();
}



