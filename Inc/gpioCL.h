/*
 * gpioCL.h
 *
 *  Created on: Nov 16, 2022
 *      Author: Marcin
 */

#ifndef GPIOCL_H_
#define GPIOCL_H_

#include "stm32f103xb.h"
#include "ioBaseCL.h"

enum MODE
{
	ANALOG_MODE, FLOATING_INPUT, IN_PULL_UP_DOWN,
	OUT_PUSH_PULL, OUT_OPEN_DRAIN, AF_PUSH_PULL, AF_OPEN_DRAIN
};

enum SPEED
{
	IN_MODE ,SP_10_MHZ, SP_2_MHZ, SP_50_MHZ
};

class pGPIO : public pIO
{
private:
	GPIO_TypeDef* m_gpio = nullptr;
	MODE m_mode;
	SPEED m_speed;
	uint8_t m_pinNr;
public:
	// Constructors.
	pGPIO() {};
	pGPIO(GPIO_TypeDef* gpioCon, MODE mode, SPEED speed, uint8_t pin_nr);
	void setupGPIO(GPIO_TypeDef*, MODE, SPEED, uint8_t);

	// Common r/w methods from the core I/O class.
	unsigned int read(void);

	void toggle();
	void write();
	void reset();
	// GPIO-specific methods.
	// Register modification methods; platform-specific
private:
	inline void in_analog();
	inline void in_floating();
	inline void in_pull_up_down();

	inline void out_push_pull();
	inline void out_open_drain();
	inline void af_push_pull();
	inline void af_open_drain();

	inline void in_mode();
	inline void sp_10_mhz();
	inline void sp_2_mhz();
	inline void sp_50_mhz();
};

#endif /* GPIOCL_H_ */
