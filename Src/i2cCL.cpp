/*
 * i2cCL.cpp
 *
 *  Created on: Nov 16, 2022
 *      Author: Marcin
 */
#include "i2cCL.h"
#include "gpioCL.h"

pI2C::pI2C(I2C_TypeDef* i2c_arg)
{
	setup_I2C(i2c_arg);
}

void pI2C::setup_I2C(I2C_TypeDef* i2c_arg)
{
	m_i2c = i2c_arg;
	__disable_irq();
	enable_reg = &(RCC->APB2ENR);
	enable_i2c_reg = &(RCC->APB1ENR);
	enable_bit = RCC_APB2ENR_IOPBEN;
	enable_i2c_bit = RCC_APB1ENR_I2C2EN;
	status = pSTATUS_ON;
	clock_en();

	if(m_i2c == I2C2)
	{
		pGPIO SCL(GPIOB, MODE::AF_OPEN_DRAIN, SPEED::SP_10_MHZ, 10);
		pGPIO SDA(GPIOB, MODE::AF_OPEN_DRAIN, SPEED::SP_10_MHZ, 11);
	}
	//reset the i2c
	m_i2c->CR1 |= I2C_CR1_SWRST;
	//normal operation
	m_i2c->CR1 &= ~I2C_CR1_SWRST;

	//set clock to 100 KHz (fclk=32 MHz)
	m_i2c->CR2 |= (32 << 0);	//32 MHz in FREQ bits
	m_i2c->CCR |= 160;		//normal i2c 100KHZ ~ 10us; clock 32 MHZ ~ 31,25 ns; CCR = (10 us / 2)/31,25 ns

	m_i2c->TRISE = 32+1;		//1000ns/31,25ns = 32

	//peripheral enable
	m_i2c->CR1 |= I2C_CR1_PE;

	__enable_irq();
}

void pI2C::clock_en()
{
	if (status == pSTATUS_ERR) { return; }
	*enable_reg |= enable_bit;
	*enable_i2c_reg |= enable_i2c_bit;
	status = pSTATUS_ON;
}

void pI2C::i2c_start()
{
	m_i2c->CR1 |= I2C_CR1_ACK;
	m_i2c->CR1 |= I2C_CR1_START;
	while(!(m_i2c->SR1 & I2C_SR1_SB));		//important!!!
}

void pI2C::i2c_write(uint8_t data)
{
	while(!(m_i2c->SR1 & I2C_SR1_TXE));
	m_i2c->DR = data;
	while(!(m_i2c->SR1 & I2C_SR1_BTF));
}

uint8_t pI2C::i2c_send_address(uint8_t address)
{
	m_i2c->DR = address;						//send the address
	while(!(m_i2c->SR1 & I2C_SR1_ADDR))		//wait for ADDR bit to set
	{
		if(m_i2c->SR1 & I2C_SR1_AF)			//check if ack failure
		{
			i2c_stop();
			return I2CStatus::ACK_ADDRESS_FAILURE;
		}
	}
	uint8_t temp = m_i2c->SR1 | m_i2c->SR2;		//read SR1 and SR2 to clear the ADDR bit
	return I2CStatus::OK;
}

void pI2C::i2c_read(uint8_t address, uint8_t* buffer, uint8_t size)
{
	int remaining = size;

	/**** STEP 1 ****/
		if (size == 1)
		{
			/**** STEP 1-a ****/
			m_i2c->DR = address | 0x01;  //  send the address (with R/W bit high!!! R = 1, W = 0)
			while (!(m_i2c->SR1 & I2C_SR1_ADDR));  // wait for ADDR bit to set

			/**** STEP 1-b ****/
			m_i2c->CR1 &= ~I2C_CR1_ACK;  // clear the ACK bit
			uint8_t temp = m_i2c->SR1 | m_i2c->SR2;  // read SR1 and SR2 to clear the ADDR bit.... EV6 condition
			m_i2c->CR1 |= I2C_CR1_STOP;  // Stop I2C

			/**** STEP 1-c ****/
			while (!(m_i2c->SR1 & I2C_SR1_RXNE));	// wait for RxNE to set

			/**** STEP 1-d ****/
			buffer[size-remaining] = m_i2c->DR;  // Read the data from the DATA REGISTER

		}

	/**** STEP 2 ****/
		else
		{
			/**** STEP 2-a ****/
			m_i2c->DR = address | 0x01;  //  send the address (same as above)
			while (!(m_i2c->SR1 & I2C_SR1_ADDR));  // wait for ADDR bit to set

			/**** STEP 2-b ****/
			uint8_t temp = m_i2c->SR1 | m_i2c->SR2;  // read SR1 and SR2 to clear the ADDR bit

			while (remaining>2)
			{
				/**** STEP 2-c ****/
				while (!(m_i2c->SR1 & I2C_SR1_RXNE));  // wait for RxNE to set

				/**** STEP 2-d ****/
				buffer[size-remaining] = m_i2c->DR;  // copy the data into the buffer

				/**** STEP 2-e ****/
				m_i2c->CR1 |= I2C_CR1_ACK;  // Set the ACK bit to Acknowledge the data received

				remaining--;
			}

			// Read the SECOND LAST BYTE
			while (!(m_i2c->SR1 & I2C_SR1_RXNE));  // wait for RxNE to set
			buffer[size-remaining] = m_i2c->DR;

			/**** STEP 2-f ****/
			m_i2c->CR1 &= ~I2C_CR1_ACK;  // clear the ACK bit

			/**** STEP 2-g ****/
			m_i2c->CR1 |= I2C_CR1_STOP;  // Stop I2C

			remaining--;

			// Read the Last BYTE
			while (!(m_i2c->SR1 & I2C_SR1_RXNE));  // wait for RxNE to set
			buffer[size-remaining] = m_i2c->DR;  // copy the data into the buffer
		}
}

void pI2C::i2c_stop()
{
	m_i2c->CR1 |= I2C_CR1_STOP;
}

void pI2C::i2c_write_dev(uint8_t address, uint8_t reg, uint8_t data)
{
	i2c_start();
	if(i2c_send_address(address) == I2CStatus::OK)
	{
		i2c_write(reg);
		i2c_write(data);
		i2c_stop();
	}
}

void pI2C::i2c_read_dev(uint8_t address, uint8_t reg, uint8_t* buffer, uint8_t size)
{
	i2c_start();
	if(i2c_send_address(address) == I2CStatus::OK)
	{
		i2c_write(reg);
		i2c_start();
		i2c_read(address, buffer, size);
		i2c_stop();
	}
}
