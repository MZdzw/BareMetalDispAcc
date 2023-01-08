/*
 * i2cCL.cpp
 *
 *  Created on: Nov 16, 2022
 *      Author: Marcin
 */
#include "i2cCL.h"

pI2C::pI2C():m_interruptType(I2CIrq::NONE),	m_currentStateIrq(I2CState::STATE_NONE){}

pI2C::pI2C(I2C_TypeDef* i2c_arg)
:m_interruptType(I2CIrq::NONE),
 m_currentStateIrq(I2CState::STATE_NONE)
{
	setupI2C(i2c_arg, I2CIrq::NONE);
}

pI2C::pI2C(I2C_TypeDef* i2c_arg, I2CIrq interruptType)
:m_interruptType(interruptType),
 m_currentStateIrq(I2CState::STATE_NONE)
{
	setupI2C(i2c_arg, interruptType);
	if(m_interruptType != I2CIrq::NONE)
		enableIrq();
}

void pI2C::enableIrq()
{
	m_i2c->CR2 |= I2C_CR2_ITEVTEN;
	m_i2c->CR2 |= I2C_CR2_ITBUFEN;

	if(m_i2c == I2C1)
		NVIC_EnableIRQ(I2C1_EV_IRQn);
	else if(m_i2c == I2C2)
		NVIC_EnableIRQ(I2C2_EV_IRQn);
	m_currentStateIrq = I2CState::STATE_NONE;
}

void pI2C::disableIrq()
{
	m_i2c->CR2 &= ~I2C_CR2_ITEVTEN;
	m_i2c->CR2 &= ~I2C_CR2_ITBUFEN;
//	if(m_i2c == I2C1)
//		NVIC_DisableIRQ(I2C1_EV_IRQn);
//	else if(m_i2c == I2C2)
//		NVIC_DisableIRQ(I2C2_EV_IRQn);
}

void pI2C::setupI2C(I2C_TypeDef* i2c_arg, I2CIrq interruptType)
{
	this->m_interruptType = interruptType;
	this->m_currentStateIrq = I2CState::STATE_NONE;
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
		SCL.setupGPIO(GPIOB, MODE::AF_OPEN_DRAIN, SPEED::SP_2_MHZ, 10);
		SDA.setupGPIO(GPIOB, MODE::AF_OPEN_DRAIN, SPEED::SP_2_MHZ, 11);
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

	if(m_interruptType != I2CIrq::NONE)
		enableIrq();
	__enable_irq();

	g_i2c2_handler.connect<&pI2C::isr>(this);
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

void pI2C::i2c_write()
{
	while(!(m_i2c->SR1 & I2C_SR1_TXE));
	m_i2c->DR = m_dataToWrite;
	while(!(m_i2c->SR1 & I2C_SR1_BTF));
}

uint8_t pI2C::i2c_send_address()
{
	m_i2c->DR = m_devAddress;						//send the address
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

void pI2C::i2c_read()
{
	int remaining = m_bytesToRead;

	/**** STEP 1-a ****/
	//common for all cases
	m_i2c->DR = m_devAddress | 0x01;  //  send the address (with R/W bit high!!! R = 1, W = 0)
	while(!(m_i2c->SR1 & I2C_SR1_ADDR));  // wait for ADDR bit to set

	if(m_bytesToRead == 1)
	{
		/**** STEP 1-b ****/
		m_i2c->CR1 &= ~I2C_CR1_ACK;  // clear the ACK bit
		uint8_t temp = m_i2c->SR1 | m_i2c->SR2;  // read SR1 and SR2 to clear the ADDR bit.... EV6 condition
		m_i2c->CR1 |= I2C_CR1_STOP;  // Stop I2C

		/**** STEP 1-c ****/
		while (!(m_i2c->SR1 & I2C_SR1_RXNE));	// wait for RxNE to set

		/**** STEP 1-d ****/
		m_buffer[m_bytesToRead-remaining] = m_i2c->DR;  // Read the data from the DATA REGISTER

	}
	else if(m_bytesToRead == 2)
	{
		uint8_t temp = m_i2c->SR1 | m_i2c->SR2;  // read SR1 and SR2 to clear the ADDR bit.... EV6 condition

		// Read the SECOND LAST BYTE
		while (!(m_i2c->SR1 & I2C_SR1_RXNE));  // wait for RxNE to set
		m_buffer[m_bytesToRead-remaining] = m_i2c->DR;

		/**** STEP 2-f ****/
		m_i2c->CR1 &= ~I2C_CR1_ACK;  // clear the ACK bit

		/**** STEP 2-g ****/
		m_i2c->CR1 |= I2C_CR1_STOP;  // Stop I2C

		remaining--;

		// Read the Last BYTE
		while (!(m_i2c->SR1 & I2C_SR1_RXNE));  // wait for RxNE to set
		m_buffer[m_bytesToRead-remaining] = m_i2c->DR;  // copy the data into the buffer
		m_i2c->CR1 |= I2C_CR1_STOP;
	}
	else
	{
		/**** STEP 2-b ****/
		uint8_t temp = m_i2c->SR1 | m_i2c->SR2;  // read SR1 and SR2 to clear the ADDR bit

		while (remaining>3)
		{
			/**** STEP 2-c ****/
			while (!(m_i2c->SR1 & I2C_SR1_RXNE));  // wait for RxNE to set

			/**** STEP 2-e ****/
			m_i2c->CR1 |= I2C_CR1_ACK;  // Set the ACK bit to Acknowledge the data received

			/**** STEP 2-d ****/
			m_buffer[m_bytesToRead-remaining] = m_i2c->DR;  // copy the data into the buffer

			remaining--;
		}
		// There is 3 bytes to read
		//it is better, cause it's according to st reference manual, not yt tutorial
		while(!(m_i2c->SR1 & I2C_SR1_RXNE));
		while(!(m_i2c->SR1 & I2C_SR1_BTF));
		m_i2c->CR1 &= ~I2C_CR1_ACK;  // clear the ACK bit
		m_buffer[m_bytesToRead-remaining] = m_i2c->DR;
		remaining--;
		m_i2c->CR1 |= I2C_CR1_STOP;  // Stop I2C
		m_buffer[m_bytesToRead-remaining] = m_i2c->DR;
		remaining--;
		while(!(m_i2c->SR1 & I2C_SR1_RXNE));
		m_buffer[m_bytesToRead-remaining] = m_i2c->DR;
	}
}

void pI2C::i2c_stop()
{
	m_i2c->CR1 |= I2C_CR1_STOP;
}

void pI2C::i2c_write_dev(uint8_t address, uint8_t reg, uint8_t data)
{
	this->m_devAddress = address;
	this->m_regAddress = reg;

	if(m_interruptType == I2CIrq::RXIRQ)
		disableIrq();
	i2c_start();
	if(i2c_send_address() == I2CStatus::OK)
	{
		this->m_dataToWrite = reg;
		i2c_write();
		this->m_dataToWrite = data;
		i2c_write();
		i2c_stop();
	}
	//if(m_interruptType == I2CIrq::RXIRQ)
		//enableIrq();
}

void pI2C::i2cReadDev(uint8_t address, uint8_t reg, uint8_t size)
{
	this->m_devAddress = address;
	this->m_dataToWrite = reg;
	this->m_bytesToRead = size;

	i2c_start();
	if(i2c_send_address() == I2CStatus::OK)
	{
		i2c_write();
		i2c_start();
		i2c_read();
		//i2c_stop();
	}
}

void pI2C::i2cReadDevIT(uint8_t address, uint8_t reg, uint8_t size)
{
	//enable interrupts
	m_i2c->CR2 |= I2C_CR2_ITEVTEN;
	m_i2c->CR2 |= I2C_CR2_ITBUFEN;
	//You only have to trigger I2C communication. Each next step is done in isr
	m_i2c->CR1 |= I2C_CR1_ACK;
	m_i2c->CR1 |= I2C_CR1_START;
	m_currentStateIrq = I2CState::START;
	m_devAddress = address;
	m_dataToWrite = reg;
	m_bytesToRead = size;
	m_remainingToRead = size;
}

void pI2C::isr()
{
	if(m_interruptType == I2CIrq::RXIRQ && m_currentStateIrq != I2CState::STATE_NONE)
	{
		if(m_i2c->SR1 & I2C_SR1_SB)
		{
			if(m_currentStateIrq == I2CState::START)
			{
				m_currentStateIrq = I2CState::ADDRESS;
				m_i2c->DR = m_devAddress;
			}
			else if(m_currentStateIrq == I2CState::REPEATED_START)
			{
				m_currentStateIrq = I2CState::READ;
				m_i2c->DR = m_devAddress | 0x01;
			}
		}
		else if(m_i2c->SR1 & I2C_SR1_ADDR)
		{
			uint8_t temp = m_i2c->SR1 | m_i2c->SR2;
			if(m_currentStateIrq == I2CState::REPEATED_START)
			{
				m_currentStateIrq = I2CState::READ;
				if(m_bytesToRead == 1)
				{
					m_i2c->CR1 &= ~I2C_CR1_ACK;
					m_i2c->CR1 |= I2C_CR1_STOP;
				}
			}
		}
		else if((m_i2c->SR1 & I2C_SR1_TXE) && (m_currentStateIrq < I2CState::WRITE))	//after write one byte there is no need to get here
		{
			m_currentStateIrq = I2CState::WRITE;
			m_i2c->DR = m_dataToWrite;
			//disable TXE and RXNE interrupts for a while
			m_i2c->CR2 &= ~I2C_CR2_ITBUFEN;
		}
		else if(m_i2c->SR1 & I2C_SR1_BTF && m_currentStateIrq == I2CState::WRITE)
		{
			m_i2c->CR1 |= I2C_CR1_ACK;
			m_i2c->CR1 |= I2C_CR1_START;
			m_currentStateIrq = I2CState::REPEATED_START;
			m_i2c->CR2 |= I2C_CR2_ITBUFEN;
		}
		else if(m_i2c->SR1 & I2C_SR1_RXNE && m_currentStateIrq == I2CState::READ)
		{
			if(m_bytesToRead > 3)
			{
				if(m_remainingToRead == 1)
				{
					m_i2c->CR1 &= ~I2C_CR1_ACK;
					m_i2c->CR1 |= I2C_CR1_STOP;
					m_currentStateIrq =  I2CState::STOP;
				}
				else
				{
					m_i2c->CR1 |= I2C_CR1_ACK;
				}
				m_buffer[m_bytesToRead-m_remainingToRead] = m_i2c->DR;
				m_remainingToRead--;
			}
		}
		else if(m_currentStateIrq == I2CState::STOP)
		{
			m_currentStateIrq =  I2CState::STATE_NONE;
			m_spare = m_i2c->DR;
			//disable interrupt
			m_i2c->CR2 &= ~I2C_CR2_ITEVTEN;
			m_i2c->CR2 &= ~I2C_CR2_ITBUFEN;
		}
	}
}

void I2C2_EV_IRQHandler()
{
	g_i2c2_handler();
}
