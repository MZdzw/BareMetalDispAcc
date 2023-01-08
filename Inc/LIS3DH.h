/*
 * LIS3DH.h
 *
 *  Created on: Jun 30, 2021
 *      Author: Marcin
 */

#ifndef INC_LIS3DH_H_
#define INC_LIS3DH_H_
#include <stdint.h>
#include "i2cCL.h"
#include <math.h>

static const uint8_t STATUS_ZYXOR = 0x80;
static const uint8_t STATUS_ZOR = 0x40;
static const uint8_t STATUS_YOR = 0x20;
static const uint8_t STATUS_XOR = 0x10;
static const uint8_t STATUS_ZYXDA = 0x08;
static const uint8_t STATUS_ZDA = 0x04;
static const uint8_t STATUS_YDA = 0x02;
static const uint8_t STATUS_XDA = 0x01;

static const uint8_t STATUS_AUX_321OR = 0x80;
static const uint8_t STATUS_AUX_3OR = 0x40;
static const uint8_t STATUS_AUX_2OR = 0x20;
static const uint8_t STATUS_AUX_1OR = 0x10;
static const uint8_t STATUS_AUX_321DA = 0x08;
static const uint8_t STATUS_AUX_3DA = 0x04;
static const uint8_t STATUS_AUX_2DA = 0x02;
static const uint8_t STATUS_AUX_1DA = 0x01;

static const uint8_t CTRL_REG1_ODR3 = 0x80;
static const uint8_t CTRL_REG1_ODR2 = 0x40;
static const uint8_t CTRL_REG1_ODR1 = 0x20;
static const uint8_t CTRL_REG1_ODR0 = 0x10;
static const uint8_t CTRL_REG1_LPEN = 0x08;
static const uint8_t CTRL_REG1_ZEN = 0x04;
static const uint8_t CTRL_REG1_YEN = 0x02;
static const uint8_t CTRL_REG1_XEN = 0x01;

static const uint8_t RATE_1_HZ   = 0x10;
static const uint8_t RATE_10_HZ  = 0x20;
static const uint8_t RATE_25_HZ  = 0x30;
static const uint8_t RATE_50_HZ  = 0x40;
static const uint8_t RATE_100_HZ = 0x50;
static const uint8_t RATE_200_HZ = 0x60;
static const uint8_t RATE_400_HZ = 0x70;

static const uint8_t CTRL_REG2_HPM1 = 0x80;
static const uint8_t CTRL_REG2_HPM0 = 0x40;
static const uint8_t CTRL_REG2_HPCF2 = 0x20;
static const uint8_t CTRL_REG2_HPCF1 = 0x10;
static const uint8_t CTRL_REG2_FDS = 0x08;
static const uint8_t CTRL_REG2_HPCLICK = 0x04;
static const uint8_t CTRL_REG2_HPIS2 = 0x02;
static const uint8_t CTRL_REG2_HPIS1 = 0x01;


static const uint8_t CTRL_REG3_I1_CLICK = 0x80;
static const uint8_t CTRL_REG3_I1_INT1 = 0x40;
static const uint8_t CTRL_REG3_I1_DRDY = 0x10;
static const uint8_t CTRL_REG3_I1_WTM = 0x04;
static const uint8_t CTRL_REG3_I1_OVERRUN = 0x02;

static const uint8_t CTRL_REG4_BDU = 0x80;
static const uint8_t CTRL_REG4_BLE = 0x40;
static const uint8_t CTRL_REG4_FS1 = 0x20;
static const uint8_t CTRL_REG4_FS0 = 0x10;
static const uint8_t CTRL_REG4_HR = 0x08;
static const uint8_t CTRL_REG4_ST1 = 0x04;
static const uint8_t CTRL_REG4_ST0 = 0x02;
static const uint8_t CTRL_REG4_SIM = 0x01;


static const uint8_t CTRL_REG5_BOOT = 0x80;
static const uint8_t CTRL_REG5_FIFO_EN = 0x40;
static const uint8_t CTRL_REG5_LIR_INT1 = 0x08;
static const uint8_t CTRL_REG5_D4D_INT1 = 0x04;

static const uint8_t CTRL_REG6_I2_CLICK = 0x80;
static const uint8_t CTRL_REG6_I2_INT2 = 0x40;
static const uint8_t CTRL_REG6_BOOT_I2 = 0x10;
static const uint8_t CTRL_REG6_H_LACTIVE = 0x02;

static const uint8_t INT1_CFG_AOI = 0x80;
static const uint8_t INT1_CFG_6D = 0x40;
static const uint8_t INT1_CFG_ZHIE_ZUPE = 0x20;
static const uint8_t INT1_CFG_ZLIE_ZDOWNE = 0x10;
static const uint8_t INT1_CFG_YHIE_YUPE = 0x08;
static const uint8_t INT1_CFG_YLIE_YDOWNE = 0x04;
static const uint8_t INT1_CFG_XHIE_XUPE = 0x02;
static const uint8_t INT1_CFG_XLIE_XDOWNE = 0x01;

static const uint8_t INT1_SRC_IA = 0x40;
static const uint8_t INT1_SRC_ZH = 0x20;
static const uint8_t INT1_SRC_ZL = 0x10;
static const uint8_t INT1_SRC_YH = 0x08;
static const uint8_t INT1_SRC_YL = 0x04;
static const uint8_t INT1_SRC_XH = 0x02;
static const uint8_t INT1_SRC_XL = 0x01;

static const uint8_t TEMP_CFG_ADC_PD = 0x80;
static const uint8_t TEMP_CFG_TEMP_EN = 0x40;

static const uint8_t FIFO_CTRL_BYPASS = 0x00;
static const uint8_t FIFO_CTRL_FIFO = 0x40;
static const uint8_t FIFO_CTRL_STREAM = 0x80;
static const uint8_t FIFO_CTRL_STREAM_TO_FIFO = 0xc0;

static const uint8_t FIFO_SRC_WTM = 0x80;
static const uint8_t FIFO_SRC_OVRN = 0x40;
static const uint8_t FIFO_SRC_EMPTY = 0x20;
static const uint8_t FIFO_SRC_FSS_MASK = 0x1f;

class LIS3DH
{
private:
	pI2C i2c_LIS3DH;
	uint8_t m_devAddress;

	uint8_t* m_reg;
	uint8_t m_numBytes;
	I2CIrq m_i2cMode;

	void writeData();
	void readData();
	void setup();
public:
	LIS3DH(uint8_t);
	uint8_t getI2Caddress() const;

	void readAllAcc();
	void readXAcc();
	void readYAcc();
	void readZAcc();

	uint16_t get_X_Acc() const;
	uint16_t get_Y_Acc() const;
	uint16_t get_Z_Acc() const;

private:
	//list of LIS3DH registers in proper order
	uint8_t STATUS_REG_AUX;
	uint8_t OUT_ADC1_L;
	uint8_t OUT_ADC1_H;
	uint8_t OUT_ADC2_L;
	uint8_t OUT_ADC2_H;
	uint8_t OUT_ADC3_L;
	uint8_t OUT_ADC3_H;
	uint8_t RESERVED1;
	uint8_t WHO_AM_I;
	uint8_t RESERVED[14];
	uint8_t CTRL_REG0;
	uint8_t TEMP_CFG_REG;
	uint8_t CTRL_REG1;
	uint8_t CTRL_REG2;
	uint8_t CTRL_REG3;
	uint8_t CTRL_REG4;
	uint8_t CTRL_REG5;
	uint8_t CTRL_REG6;
	uint8_t REFERENCE;
	uint8_t STATUS_REG;
	uint8_t OUT_X_L;
	uint8_t OUT_X_H;
	uint8_t OUT_Y_L;
	uint8_t OUT_Y_H;
	uint8_t OUT_Z_L;
	uint8_t OUT_Z_H;
	uint8_t FIFO_CTRL_REG;
	uint8_t FIFO_SRC_REG;
	uint8_t INT1_CFG;
	uint8_t INT1_SRC;
	uint8_t INT1_THS;
	uint8_t INT1_DURATION;
	uint8_t INT2_CFG;
	uint8_t INT2_SRC;
	uint8_t INT2_THS;
	uint8_t INT2_DURATION;
	uint8_t CLICK_CFG;
	uint8_t CLICK_SRC;
	uint8_t CLICK_THS;
	uint8_t TIME_LIMIT;
	uint8_t TIME_LATENCY;
	uint8_t TIME_WINDOW;
	uint8_t ACT_THS;
	uint8_t ACT_DUR;
};



#endif /* INC_LIS3DH_H_ */
