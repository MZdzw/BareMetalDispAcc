/*
 * i2cCL.h
 *
 *  Created on: Nov 16, 2022
 *      Author: Marcin
 */
#ifndef I2CCL_H_
#define I2CCL_H_

#include "stm32f103xb.h"
#include "ioBaseCL.h"
#include "interruptCL.h"
#include "gpioCL.h"

static InterruptHandler g_i2c2_handler;

#ifdef __cplusplus
extern "C" {
#endif

void I2C2_EV_IRQHandler();

#ifdef __cplusplus
}
#endif

enum I2CStatus
{
	OK, ACK_ADDRESS_FAILURE
};

enum I2CIrq
{
	NONE, TXIRQ, RXIRQ, ALL
};

enum I2CState
{
	START, ADDRESS, WRITE, REPEATED_START, READ, STOP, STATE_NONE
};

class pI2C : public pIO
{
public:
	pI2C();
	pI2C(I2C_TypeDef*);
	pI2C(I2C_TypeDef*, I2CIrq);
	void setupI2C(I2C_TypeDef*, I2CIrq);

	void clock_en(void) override;

	uint8_t* getBufferAddr()	{ return m_buffer; }

	void i2c_write_dev(uint8_t address, uint8_t reg, uint8_t data);

	void i2cReadDev(uint8_t address, uint8_t reg, uint8_t size);
	void i2cReadDevIT(uint8_t address, uint8_t reg, uint8_t size);
private:
	void isr();
	void enableIrq();
	void disableIrq();

	void i2c_start();
	void i2c_write();
	uint8_t i2c_send_address();
	void i2c_read();
	void i2c_stop();

	//i2c transfer parameters (devAddres, registerAddres, dataToSend, buffer and its address)
	uint8_t m_devAddress;
	uint8_t m_regAddress;
	uint8_t m_dataToWrite;
	uint8_t m_buffer[6];		//assumed that 6 bytes is the most message to receive
	uint8_t m_bytesToRead;
	uint8_t m_remainingToRead;
	uint8_t m_spare;


	//i2c handle
	I2C_TypeDef *m_i2c;
	//I2C pins
	pGPIO SCL;
	pGPIO SDA;

	//enable bit/registers
	volatile uint32_t *enable_i2c_reg = 0;
	uint32_t enable_i2c_bit = 0;

	//interrupt variables
	I2CIrq m_interruptType;
	I2CState m_currentStateIrq;
};

#endif /* I2CCL_H_ */
