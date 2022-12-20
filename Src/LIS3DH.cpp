/*
 * LIS3DH.cpp
 *
 *  Created on: 11 gru 2022
 *      Author: Marcin
 */
#include "LIS3DH.h"
#include "i2cCL.h"

LIS3DH::LIS3DH(uint8_t LIS3DH_i2cAdd)
:m_address(LIS3DH_i2cAdd)
{
	i2c_LIS3DH.setup_I2C(I2C2);
	setup_registers_adddress();
	setup();
}

uint8_t LIS3DH::setup()
{
	//set low power mode 1 Hz, XYZ detection
	m_CTRL_REG1.reg_value = CTRL_REG1_ODR1 | CTRL_REG1_LPEN | CTRL_REG1_ZEN | CTRL_REG1_YEN | CTRL_REG1_XEN;
	//high pass filter, leave it as it is (no filtering)
	//reg2 =
	//reg3 =
	writeregister8(m_CTRL_REG1.reg_address, m_CTRL_REG1.reg_value);
}

void LIS3DH::writeData(uint8_t addr, const uint8_t *buf, size_t numBytes)
{
	if(numBytes > 1)
	{
		addr |= 0x80;	//wysyl kilku bajtow pod rzad - MSB = 1
	}
	i2c_LIS3DH.i2c_write_dev((get_I2Caddress()<<1), addr, *buf);
	//HAL_I2C_Mem_Write(&hi2c1, (getI2Caddr()<<1), addr, 1, buf, numBytes, 100);
	//HAL_I2C_Master_Transmit(hi2c, DevAddress, pData, Size, Timeout);
}

void LIS3DH::writeregister8(uint8_t addr, uint8_t val)
{
	writeData(addr,&val,sizeof(val));
}

void LIS3DH::writeregister16(uint8_t addr, uint16_t val)
{
	uint8_t req[2];
	req[0] = val & 0xff;
	req[1] = val >> 8;

	writeData(addr, req, sizeof(req));
}

uint8_t LIS3DH::get_I2Caddress() const
{
	return m_address;
}


void LIS3DH::readData(uint8_t addr, uint8_t *buf, size_t numBytes)
{
	if(numBytes > 1)
	{
			addr |= 0x80;	//wysyl kilku bajtow pod rzad - MSB = 1
	}
	//HAL_I2C_Mem_Read(&hi2c1, (getI2Caddr()<<1), addr, 1, buf, numBytes, 100);
	i2c_LIS3DH.i2c_read_dev((get_I2Caddress()<<1), addr, buf, numBytes);
}

uint8_t LIS3DH::readregister8(uint8_t addr)
{
	uint8_t read_val[1];
	readData(addr, read_val, sizeof(read_val));

	return read_val[0];
}

uint16_t LIS3DH::readregister16(uint8_t addr)
{
	uint8_t read_val[2];
	readData(addr, read_val, sizeof(read_val));

	return read_val[0] | (((uint16_t)read_val[1]) << 8);
}

void LIS3DH::read_Acc()
{
	uint8_t res[6];
	readData((&OUT_X_L - &STATUS_REG_AUX + 7), res, sizeof(res));

	m_x = res[1];
	m_y = res[3];
	m_z = res[5];
}

uint8_t LIS3DH::setup_registers_adddress()
{
	m_CTRL_REG1.reg_address = 0x20;
}

uint8_t LIS3DH::get_X_Acc() const
{
	return m_x;
}

uint8_t LIS3DH::get_Y_Acc() const
{
	return m_y;
}

uint8_t LIS3DH::get_Z_Acc() const
{
	return m_z;
}
