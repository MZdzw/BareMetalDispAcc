/*
 * spiCL.h
 *
 *  Created on: Jan 8, 2023
 *      Author: marzdz
 */
#ifndef SPICL_H_
#define SPICL_H_

#include "stm32f103xb.h"
#include "ioBaseCL.h"
#include "gpioCL.h"

class pSPI : public pIO
{
private:
	SPI_TypeDef* m_spi;
	//enable bit/registers
	volatile uint32_t *enableSpiReg = 0;
	uint32_t enableSpiBit = 0;

	uint8_t m_buffer[10];		//10 max bytes to write at once
	uint8_t m_numBytes;			//size of bytes to transmit/receive

	void clock_en(void) override;

	//declare SPI pins
	pGPIO NSS;
	pGPIO SCK;
	pGPIO MISO;
	pGPIO MOSI;
public:
	pSPI() {};
	pSPI(SPI_TypeDef*);
	void setupSPI(SPI_TypeDef*);
	void dataTransmit(uint8_t*, uint8_t);


};

#endif /* SPICL_H_ */
