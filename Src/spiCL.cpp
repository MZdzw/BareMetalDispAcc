/*
 * spiCL.cpp
 *
 *  Created on: Jan 8, 2023
 *      Author: marzdz
 */

#include <stdio.h>
#include "spiCL.h"


pSPI::pSPI(SPI_TypeDef* spi_arg)
{
	m_spi = spi_arg;
	enable_reg = &(RCC->APB2ENR);
	enable_bit = RCC_APB2ENR_IOPBEN;
	if(m_spi == SPI1)
		enableSpiReg = &(RCC->APB2ENR);
	else
		enableSpiReg = &(RCC->APB1ENR);
	enableSpiBit = RCC_APB1ENR_SPI2EN;
	status = pSTATUS_ON;
	__disable_irq();
	clock_en();

	if(m_spi == SPI2)
	{
		//check table with configurations
		//NSS.setupGPIO(GPIOB, MODE::OUT_PUSH_PULL, SPEED::SP_2_MHZ, 12);
		SCK.setupGPIO(GPIOB, MODE::AF_PUSH_PULL, SPEED::SP_10_MHZ, 13);
		MISO.setupGPIO(GPIOB, MODE::FLOATING_INPUT, SPEED::SP_10_MHZ, 14);
		MOSI.setupGPIO(GPIOB, MODE::AF_PUSH_PULL, SPEED::SP_10_MHZ, 15);
	}
	// 1. Select BR[2:0} to define serial clock baudrate (SPI_CR1)
	// typical SPI baudrate is 1 MHZ	fclk = 32 MHZ, BR = fpclk/32 = 100
	m_spi->CR1 |= SPI_CR1_BR_1;
	// 2. Select CPOL and CPHO to define spi mode (e.g. rising/falling edge)
	//first edge transition and capture data on rising edge
	m_spi->CR1 &= ~(SPI_CR1_CPHA | SPI_CR1_CPOL);
	// 3. Set the DFF bit to define 8 or 16 bit frame
	// 8 bit format
	m_spi->CR1 &= ~(SPI_CR1_DFF);
	// 4. Configure LSBFirst bit in the SPI_CR1 to define frame format
	// MSB bit transmit first
	m_spi->CR1 &= ~(SPI_CR1_LSBFIRST);
	// 5. Set SSOE bit for NSS pin for output mode
	// SS (slave select) is enabled in master mode
	//m_spi->CR2 |= SPI_CR2_SSOE;
	m_spi->CR1 |= SPI_CR1_SSM;
	m_spi->CR1 |= SPI_CR1_SSI;
	// 6. MSTR and SPE bits must be set (they remain high only if the NSS pin is set)
	// Master configuration
	m_spi->CR1 |= SPI_CR1_MSTR;
	// Enable SPI	(enabled is done in dataTransmit function; makes more sense)
	m_spi->CR1 |= SPI_CR1_SPE;
	//pGPIO NSS(GPIOB, MODE::OUT_PUSH_PULL, SPEED::SP_2_MHZ, 12);
	//NSS.write(static_cast<uint32_t>(GPIO_ODR_ODR12));

	// configured as half-duplex. To tranmit only (as master - MOSI) need to ignore RX buffer
	//configured as half-duplex is similar as full-duplex (only ignere RX buffer)
	__enable_irq();
}

void pSPI::setupSPI(SPI_TypeDef* spi_arg)
{
	m_spi = spi_arg;
	enable_reg = &(RCC->APB2ENR);
	enable_bit = RCC_APB2ENR_IOPBEN;
	if(m_spi == SPI1)
		enableSpiReg = &(RCC->APB2ENR);
	else
		enableSpiReg = &(RCC->APB1ENR);
	enableSpiBit = RCC_APB1ENR_SPI2EN;
	status = pSTATUS_ON;
	__disable_irq();
	clock_en();

	if(m_spi == SPI2)
	{
		//check table with configurations
		pGPIO SCK(GPIOB, MODE::AF_PUSH_PULL, SPEED::SP_10_MHZ, 13);
		pGPIO MISO(GPIOB, MODE::FLOATING_INPUT, SPEED::SP_2_MHZ, 14);
		pGPIO MOSI(GPIOB, MODE::AF_PUSH_PULL, SPEED::SP_10_MHZ, 15);
	}
	//m_spi->CR1 |= SPI_CR1_BR_2;
	m_spi->CR1 &= ~(SPI_CR1_CPHA | SPI_CR1_CPOL);
	m_spi->CR1 &= ~(SPI_CR1_DFF);

	m_spi->CR1 &= ~(SPI_CR1_LSBFIRST);
	m_spi->CR1 |= SPI_CR1_SSM;
	m_spi->CR1 |= SPI_CR1_SSI;
	m_spi->CR1 |= SPI_CR1_MSTR;
	m_spi->CR1 |= SPI_CR1_SPE;
	__enable_irq();
}

void pSPI::dataTransmit(uint8_t* buf, uint8_t size)
{
	m_numBytes = size;
	for(uint8_t i = 0; i < m_numBytes; ++i)
		m_buffer[i] = *(buf + i);
	//m_spi->CR1 |= SPI_CR1_SPE;
	for(uint8_t i = 0; i < m_numBytes; ++i)
	{
		while(!(m_spi->SR & SPI_SR_TXE));
		m_spi->DR = m_buffer[i];
	}
	while(!(m_spi->SR & SPI_SR_TXE));
	while(m_spi->SR & SPI_SR_BSY);
	//clear overrun flag (important!!!)
	uint8_t temp = m_spi->DR;
	temp = m_spi->SR;
	//m_spi->CR1 &= ~SPI_CR1_SPE;
}

void pSPI::clock_en()
{
	if (status == pSTATUS_ERR) { return; }
	*enable_reg |= enable_bit;
	*enableSpiReg |= enableSpiBit;
	status = pSTATUS_ON;
}
