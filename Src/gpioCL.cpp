/*
 * gpioCL.cpp
 *
 *  Created on: Nov 16, 2022
 *      Author: Marcin
 */
#include "gpioCL.h"

pGPIO::pGPIO(GPIO_TypeDef* gpioCon, MODE mode, SPEED speed, uint8_t pin_nr)
{
	gpio = gpioCon;
	if(gpio == GPIOC)
	{
		enable_reg = &(RCC->APB2ENR);		//enabled GPIOC port
		enable_bit = RCC_APB2ENR_IOPCEN;
	}
	//RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	status = pSTATUS_SET;
	clock_en();

	if(mode == MODE::OUT_PUSH_PULL)	//push-pull output
		out_push_pull(pin_nr);	//push-pull
	else if(mode == MODE::AF_PUSH_PULL)	//AF PUSH-PULL
		af_push_pull(pin_nr);	//AF push-pull
	else if(mode == MODE::AF_OPEN_DRAIN)	//AF OPEN-DRAIN
		af_open_drain(pin_nr);	//AF open-drain
	else if(mode == MODE::OUT_OPEN_DRAIN)
		out_open_drain(pin_nr);
	else if(mode == MODE::ANALOG_MODE)
		in_analog(pin_nr);
	else if(mode == MODE::FLOATING_INPUT)
		in_floating(pin_nr);
	else if(mode == MODE::IN_PULL_UP_DOWN)
		in_pull_up_down(pin_nr);

	if(speed == SPEED::SP_10_MHZ)
		sp_10_mhz(pin_nr);
	else if(speed == SPEED::SP_2_MHZ)
		sp_2_mhz(pin_nr);
	else if(speed == SPEED::SP_50_MHZ)
		sp_50_mhz(pin_nr);
	else if(speed == SPEED::IN_MODE)
		in_mode(pin_nr);
}

unsigned int pGPIO::read(void)
{
	if (status == pSTATUS_ERR) { return 0; }
	// Return the current value of all 16 pins.
	return gpio->IDR;
}

void pGPIO::toggle(uint32_t pin_nr)
{
	if (status == pSTATUS_ERR) { return; }

	gpio->ODR ^= pin_nr;
}

void pGPIO::write(uint32_t pin_nr)
{
	if (status == pSTATUS_ERR) { return; }

	gpio->ODR |= pin_nr;
}

void pGPIO::reset(uint32_t pin_nr)
{
	if (status == pSTATUS_ERR) { return; }

	gpio->ODR &= ~pin_nr;
}

inline void pGPIO::in_analog(uint8_t pin_nr)
{
	if(pin_nr >= 8)
	{
		pin_nr -= 8;
		gpio->CRH &= ~(1 << (pin_nr*4 + 2));
		gpio->CRH &= ~(1 << (pin_nr*4 + 3));
	}
	else
	{
		gpio->CRL &= ~(1 << (pin_nr*2 + 2));
		gpio->CRL &= ~(1 << (pin_nr*4 + 3));
	}
}

inline void pGPIO::in_floating(uint8_t pin_nr)
{
	if(pin_nr >= 8)
	{
		pin_nr -= 8;
		gpio->CRH |=  (1 << (pin_nr*4 + 2));
		gpio->CRH &= ~(1 << (pin_nr*4 + 3));
	}
	else
	{
		gpio->CRL |=  (1 << (pin_nr*2 + 2));
		gpio->CRL &= ~(1 << (pin_nr*4 + 3));
	}
}

inline void pGPIO::in_pull_up_down(uint8_t pin_nr)
{
	if(pin_nr >= 8)
	{
		pin_nr -= 8;
		gpio->CRH &= ~(1 << (pin_nr*4 + 2));
		gpio->CRH |=  (1 << (pin_nr*4 + 3));
	}
	else
	{
		gpio->CRL &= ~(1 << (pin_nr*2 + 2));
		gpio->CRL |=  (1 << (pin_nr*4 + 3));
	}
}

inline void pGPIO::out_push_pull(uint8_t pin_nr)
{
	if(pin_nr >= 8)
	{
		//push pull
		pin_nr -= 8;
		gpio->CRH &= ~(1 << (pin_nr*4 + 2));
		gpio->CRH &= ~(1 << (pin_nr*4 + 3));
	}
	else
	{
		//push pull
		gpio->CRL &= ~(1 << (pin_nr*4 + 2));
		gpio->CRL &= ~(1 << (pin_nr*4 + 3));
	}
}

inline void pGPIO::out_open_drain(uint8_t pin_nr)
{
	if(pin_nr >= 8)
	{
		pin_nr -= 8;
		gpio->CRH |=  (1 << (pin_nr*4 + 2));
		gpio->CRH &= ~(1 << (pin_nr*4 + 3));
	}
	else
	{
		gpio->CRL |=  (1 << (pin_nr*4 + 2));
		gpio->CRL &= ~(1 << (pin_nr*4 + 3));
	}
}

inline void pGPIO::af_push_pull(uint8_t pin_nr)
{
	if(pin_nr >= 8)
	{
		//push pull AF
		pin_nr -= 8;
		gpio->CRH &= ~(1 << (pin_nr*4 + 2));
		gpio->CRH |=  (1 << (pin_nr*4 + 3));
	}
	else
	{
		//push pull AF
		gpio->CRL &= ~(1 << (pin_nr*4 + 2));
		gpio->CRL |=  (1 << (pin_nr*4 + 3));
	}
}

inline void pGPIO::af_open_drain(uint8_t pin_nr)
{
	if(pin_nr >= 8)
	{
		pin_nr -= 8;
		gpio->CRH |= (1 << (pin_nr*4 + 2));
		gpio->CRH |= (1 << (pin_nr*4 + 3));
	}
	else
	{
		gpio->CRL |= (1 << (pin_nr*4 + 2));
		gpio->CRL |= (1 << (pin_nr*4 + 3));
	}
}

inline void pGPIO::in_mode(uint8_t pin_nr)
{
	if(pin_nr >= 8)
	{
		pin_nr -= 8;
		gpio->CRH &= ~(1 << (pin_nr*4));
		gpio->CRH &= ~(1 << (pin_nr*4 + 1));
	}
	else
	{
		gpio->CRL &= ~(1 << (pin_nr*4));
		gpio->CRL &= ~(1 << (pin_nr*4 + 1));
	}
}

inline void pGPIO::sp_10_mhz(uint8_t pin_nr)
{
	if(pin_nr >= 8)
	{
		pin_nr -= 8;
		gpio->CRH |=  (1 << (pin_nr*4));
		gpio->CRH &= ~(1 << (pin_nr*4 + 1));
	}
	else
	{
		gpio->CRL |=  (1 << (pin_nr*4));
		gpio->CRL &= ~(1 << (pin_nr*4 + 1));
	}
}

inline void pGPIO::sp_2_mhz(uint8_t pin_nr)
{
	if(pin_nr >= 8)
	{
		pin_nr -= 8;
		gpio->CRH &= ~(1 << (pin_nr*4));
		gpio->CRH |=  (1 << (pin_nr*4 + 1));
	}
	else
	{
		gpio->CRL &= ~(1 << (pin_nr*4));
		gpio->CRL |=  (1 << (pin_nr*4 + 1));
	}
}

inline void pGPIO::sp_50_mhz(uint8_t pin_nr)
{
	if(pin_nr >= 8)
	{
		pin_nr -= 8;
		gpio->CRH |= (1 << (pin_nr*4));
		gpio->CRH |= (1 << (pin_nr*4 + 1));
	}
	else
	{
		gpio->CRL |= (1 << (pin_nr*4));
		gpio->CRL |= (1 << (pin_nr*4 + 1));
	}
}

