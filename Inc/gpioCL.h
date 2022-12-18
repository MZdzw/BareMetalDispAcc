/*
 * gpioCL.h
 *
 *  Created on: Nov 16, 2022
 *      Author: Marcin
 */

#ifndef GPIOCL_H_
#define GPIOCL_H_

#include "stm32f103xb.h"
#include "ioBaseCl.h"

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
public:
		// Constructors.
		pGPIO(GPIO_TypeDef* gpioCon, MODE mode, SPEED speed, uint8_t pin_nr);

		// Common r/w methods from the core I/O class.
		unsigned int read(void);

		void toggle(uint32_t);
		void write(uint32_t);
		void reset(uint32_t);
		// GPIO-specific methods.
		// Register modification methods; platform-specific.

protected:
      // Reference GPIO register struct.
      GPIO_TypeDef* gpio = nullptr;
private:
      inline void in_analog(uint8_t);
      inline void in_floating(uint8_t);
      inline void in_pull_up_down(uint8_t);

      inline void out_push_pull(uint8_t);
      inline void out_open_drain(uint8_t);
      inline void af_push_pull(uint8_t);
      inline void af_open_drain(uint8_t);

      inline void in_mode(uint8_t);
      inline void sp_10_mhz(uint8_t);
      inline void sp_2_mhz(uint8_t);
      inline void sp_50_mhz(uint8_t);
};

#endif /* GPIOCL_H_ */
