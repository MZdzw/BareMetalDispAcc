/*
 * LIS3DH.cpp
 *
 *  Created on: 11 gru 2022
 *      Author: Marcin
 */
#include "LIS3DH.h"

LIS3DH::LIS3DH(uint8_t LIS3DH_i2cAdd)
:m_devAddress(LIS3DH_i2cAdd),
 m_i2cMode(I2CIrq::RXIRQ)
{
	i2c_LIS3DH.setupI2C(I2C2, m_i2cMode);
	setup();
}

void LIS3DH::setup()
{
	m_reg = &CTRL_REG1;
	//set low power mode 1 Hz, XYZ detection
	*m_reg = CTRL_REG1_ODR1 | CTRL_REG1_LPEN | CTRL_REG1_ZEN | CTRL_REG1_YEN | CTRL_REG1_XEN;
	//high pass filter, leave it as it is (no filtering)
	//reg2 =
	//reg3 =
	m_numBytes = 1;
	writeData();
}

void LIS3DH::writeData()
{
	uint32_t regAddr = m_reg - &STATUS_REG_AUX + 7;
	if(m_numBytes > 1)
	{
		regAddr |= 0x80;	//wysyl kilku bajtow pod rzad - MSB = 1
	}
	i2c_LIS3DH.i2c_write_dev((getI2Caddress()<<1), regAddr, *m_reg);
}

uint8_t LIS3DH::getI2Caddress() const
{
	return m_devAddress;
}


void LIS3DH::readData()
{
	uint32_t regAddr = m_reg - &STATUS_REG_AUX + 7;
	if(m_numBytes > 1)
	{
		regAddr |= 0x80;	//wysyl kilku bajtow pod rzad - MSB = 1
	}
	(m_i2cMode == I2CIrq::RXIRQ) ?  i2c_LIS3DH.i2cReadDevIT((getI2Caddress()<<1), regAddr,  m_numBytes) :
	i2c_LIS3DH.i2cReadDev((getI2Caddress()<<1), regAddr,  m_numBytes);
}

void LIS3DH::readAllAcc()
{
	m_reg = &OUT_X_L;
	m_numBytes = 6;

	readData();
	auto *accBuffer = i2c_LIS3DH.getBufferAddr();
	OUT_X_L = *(accBuffer + 0);
	OUT_X_H = *(accBuffer + 1);
	OUT_Y_L = *(accBuffer + 2);
	OUT_Y_H = *(accBuffer + 3);
	OUT_Z_L = *(accBuffer + 4);
	OUT_Z_H = *(accBuffer + 5);
}

void LIS3DH::readXAcc()
{
	m_reg = &OUT_X_L;
	m_numBytes = 2;

	readData();
}

void LIS3DH::readYAcc()
{
	m_reg = &OUT_Y_L;
	m_numBytes = 2;

	readData();
}

void LIS3DH::readZAcc()
{
	m_reg = &OUT_Z_L;
	m_numBytes = 2;

	readData();
}

uint16_t LIS3DH::get_X_Acc() const
{
	return (OUT_X_L << 8) | OUT_X_H;
}

uint16_t LIS3DH::get_Y_Acc() const
{
	return (OUT_Y_L << 8) | OUT_Y_H;
}

uint16_t LIS3DH::get_Z_Acc() const
{
	return (OUT_Z_L << 8) | OUT_Z_H;
}
