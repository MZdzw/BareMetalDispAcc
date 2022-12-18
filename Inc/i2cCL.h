/*
 * i2cCL.h
 *
 *  Created on: Nov 16, 2022
 *      Author: Marcin
 */

#ifndef I2CCL_H_
#define I2CCL_H_

#include "stm32f103xb.h"
#include "ioBaseCl.h"

enum I2CStatus
{
	OK, ACK_ADDRESS_FAILURE
};

class pI2C : public pIO
{
public:
	pI2C() {}
	pI2C(I2C_TypeDef*);
	void setup_I2C(I2C_TypeDef*);

	void clock_en(void) override;

	void i2c_write_dev(uint8_t address, uint8_t reg, uint8_t data);
	void i2c_read_dev(uint8_t address, uint8_t reg, uint8_t* buffer, uint8_t size);

private:
	void i2c_start();
	void i2c_write(uint8_t);
	uint8_t i2c_send_address(uint8_t);
	void i2c_read(uint8_t, uint8_t*, uint8_t);
	void i2c_stop();

	I2C_TypeDef *m_i2c;
	volatile uint32_t *enable_i2c_reg = 0;
	uint32_t enable_i2c_bit = 0;
};

#endif /* I2CCL_H_ */
