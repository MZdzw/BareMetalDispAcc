/*
 * gpioCL.cpp
 *
 *  Created on: Nov 16, 2022
 *      Author: Marcin
 */
#include "gpioCL.h"

pGPIO::pGPIO(GPIO_TypeDef* gpioCon, MODE mode, SPEED speed, uint8_t pin_nr)
: m_gpio(gpioCon), m_mode(mode), m_speed(speed), m_pinNr(pin_nr)
{
	m_gpio = gpioCon;
	if(m_gpio == GPIOA)
	{
		enable_reg = &(RCC->APB2ENR);		//enabled GPIOC port
		enable_bit = RCC_APB2ENR_IOPAEN;
	}
	else if(m_gpio == GPIOB)
	{
		enable_reg = &(RCC->APB2ENR);		//enabled GPIOC port
		enable_bit = RCC_APB2ENR_IOPBEN;
	}
	else if(m_gpio == GPIOC)
	{
		enable_reg = &(RCC->APB2ENR);		//enabled GPIOC port
		enable_bit = RCC_APB2ENR_IOPCEN;
	}
	else if(m_gpio == GPIOD)
	{
		enable_reg = &(RCC->APB2ENR);		//enabled GPIOC port
		enable_bit = RCC_APB2ENR_IOPDEN;
	}
	//RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	status = pSTATUS_SET;
	clock_en();
	m_pinNr = pin_nr;
	if(mode == MODE::OUT_PUSH_PULL)	//push-pull output
		out_push_pull();	//push-pull
	else if(mode == MODE::AF_PUSH_PULL)	//AF PUSH-PULL
		af_push_pull();	//AF push-pull
	else if(mode == MODE::AF_OPEN_DRAIN)	//AF OPEN-DRAIN
		af_open_drain();	//AF open-drain
	else if(mode == MODE::OUT_OPEN_DRAIN)
		out_open_drain();
	else if(mode == MODE::ANALOG_MODE)
		in_analog();
	else if(mode == MODE::FLOATING_INPUT)
		in_floating();
	else if(mode == MODE::IN_PULL_UP_DOWN)
		in_pull_up_down();

	if(speed == SPEED::SP_10_MHZ)
		sp_10_mhz();
	else if(speed == SPEED::SP_2_MHZ)
		sp_2_mhz();
	else if(speed == SPEED::SP_50_MHZ)
		sp_50_mhz();
	else if(speed == SPEED::IN_MODE)
		in_mode();
}

unsigned int pGPIO::read(void)
{
	if (status == pSTATUS_ERR) { return 0; }
	// Return the current value of all 16 pins.
	return m_gpio->IDR;
}

void pGPIO::toggle()
{
	if (status == pSTATUS_ERR) { return; }

	m_gpio->ODR ^= 1 << m_pinNr;
}

void pGPIO::write()
{
	if (status == pSTATUS_ERR) { return; }

	m_gpio->ODR |= 1 << m_pinNr;
}

void pGPIO::reset()
{
	if (status == pSTATUS_ERR) { return; }

	m_gpio->ODR &= ~(1 << m_pinNr);
}

inline void pGPIO::in_analog()
{
	if(m_pinNr >= 8)
	{
		m_gpio->CRH &= ~(1 << ((m_pinNr - 8)*4 + 2));
		m_gpio->CRH &= ~(1 << ((m_pinNr - 8)*4 + 3));
	}
	else
	{
		m_gpio->CRL &= ~(1 << (m_pinNr*2 + 2));
		m_gpio->CRL &= ~(1 << (m_pinNr*4 + 3));
	}
}

inline void pGPIO::in_floating()
{
	if(m_pinNr >= 8)
	{
		m_gpio->CRH |=  (1 << ((m_pinNr - 8)*4 + 2));
		m_gpio->CRH &= ~(1 << ((m_pinNr - 8)*4 + 3));
	}
	else
	{
		m_gpio->CRL |=  (1 << (m_pinNr*2 + 2));
		m_gpio->CRL &= ~(1 << (m_pinNr*4 + 3));
	}
}

inline void pGPIO::in_pull_up_down()
{
	if(m_pinNr >= 8)
	{
		m_gpio->CRH &= ~(1 << ((m_pinNr - 8)*4 + 2));
		m_gpio->CRH |=  (1 << ((m_pinNr - 8)*4 + 3));
	}
	else
	{
		m_gpio->CRL &= ~(1 << (m_pinNr*2 + 2));
		m_gpio->CRL |=  (1 << (m_pinNr*4 + 3));
	}
}

inline void pGPIO::out_push_pull()
{
	if(m_pinNr >= 8)
	{
		//push pull
		m_gpio->CRH &= ~(1 << ((m_pinNr - 8)*4 + 2));
		m_gpio->CRH &= ~(1 << ((m_pinNr - 8)*4 + 3));
	}
	else
	{
		//push pull
		m_gpio->CRL &= ~(1 << (m_pinNr*4 + 2));
		m_gpio->CRL &= ~(1 << (m_pinNr*4 + 3));
	}
}

inline void pGPIO::out_open_drain()
{
	if(m_pinNr >= 8)
	{
		m_gpio->CRH |=  (1 << ((m_pinNr - 8)*4 + 2));
		m_gpio->CRH &= ~(1 << ((m_pinNr - 8)*4 + 3));
	}
	else
	{
		m_gpio->CRL |=  (1 << (m_pinNr*4 + 2));
		m_gpio->CRL &= ~(1 << (m_pinNr*4 + 3));
	}
}

inline void pGPIO::af_push_pull()
{
	if(m_pinNr >= 8)
	{
		//push pull AF
		m_gpio->CRH &= ~(1 << ((m_pinNr - 8)*4 + 2));
		m_gpio->CRH |=  (1 << ((m_pinNr - 8)*4 + 3));
	}
	else
	{
		//push pull AF
		m_gpio->CRL &= ~(1 << (m_pinNr*4 + 2));
		m_gpio->CRL |=  (1 << (m_pinNr*4 + 3));
	}
}

inline void pGPIO::af_open_drain()
{
	if(m_pinNr >= 8)
	{
		m_gpio->CRH |= (1 << ((m_pinNr - 8)*4 + 2));
		m_gpio->CRH |= (1 << ((m_pinNr - 8)*4 + 3));
	}
	else
	{
		m_gpio->CRL |= (1 << (m_pinNr*4 + 2));
		m_gpio->CRL |= (1 << (m_pinNr*4 + 3));
	}
}

inline void pGPIO::in_mode()
{
	if(m_pinNr >= 8)
	{
		m_gpio->CRH &= ~(1 << ((m_pinNr - 8)*4));
		m_gpio->CRH &= ~(1 << ((m_pinNr - 8)*4 + 1));
	}
	else
	{
		m_gpio->CRL &= ~(1 << (m_pinNr*4));
		m_gpio->CRL &= ~(1 << (m_pinNr*4 + 1));
	}
}

inline void pGPIO::sp_10_mhz()
{
	if(m_pinNr >= 8)
	{
		m_gpio->CRH |=  (1 << ((m_pinNr - 8)*4));
		m_gpio->CRH &= ~(1 << ((m_pinNr - 8)*4 + 1));
	}
	else
	{
		m_gpio->CRL |=  (1 << (m_pinNr*4));
		m_gpio->CRL &= ~(1 << (m_pinNr*4 + 1));
	}
}

inline void pGPIO::sp_2_mhz()
{
	if(m_pinNr >= 8)
	{
		m_gpio->CRH &= ~(1 << ((m_pinNr - 8)*4));
		m_gpio->CRH |=  (1 << ((m_pinNr - 8)*4 + 1));
	}
	else
	{
		m_gpio->CRL &= ~(1 << (m_pinNr*4));
		m_gpio->CRL |=  (1 << (m_pinNr*4 + 1));
	}
}

inline void pGPIO::sp_50_mhz()
{
	if(m_pinNr >= 8)
	{
		m_gpio->CRH |= (1 << ((m_pinNr - 8)*4));
		m_gpio->CRH |= (1 << ((m_pinNr - 8)*4 + 1));
	}
	else
	{
		m_gpio->CRL |= (1 << (m_pinNr*4));
		m_gpio->CRL |= (1 << (m_pinNr*4 + 1));
	}
}

void pGPIO::setupGPIO(GPIO_TypeDef* gpioCon, MODE mode, SPEED speed, uint8_t pin_nr)
{
	m_gpio = gpioCon;
	m_mode = mode;
	m_speed = speed;
	m_pinNr = pin_nr;
	enable_reg = &(RCC->APB2ENR);
	if(m_gpio == GPIOA)
		enable_bit = RCC_APB2ENR_IOPAEN;
	else if(m_gpio == GPIOB)
		enable_bit = RCC_APB2ENR_IOPBEN;
	else if(m_gpio == GPIOC)
		enable_bit = RCC_APB2ENR_IOPCEN;
	else if(m_gpio == GPIOD)
		enable_bit = RCC_APB2ENR_IOPDEN;

	status = pSTATUS_SET;
	clock_en();

	if(mode == MODE::OUT_PUSH_PULL)	//push-pull output
		out_push_pull();	//push-pull
	else if(mode == MODE::AF_PUSH_PULL)	//AF PUSH-PULL
		af_push_pull();	//AF push-pull
	else if(mode == MODE::AF_OPEN_DRAIN)	//AF OPEN-DRAIN
		af_open_drain();	//AF open-drain
	else if(mode == MODE::OUT_OPEN_DRAIN)
		out_open_drain();
	else if(mode == MODE::ANALOG_MODE)
		in_analog();
	else if(mode == MODE::FLOATING_INPUT)
		in_floating();
	else if(mode == MODE::IN_PULL_UP_DOWN)
		in_pull_up_down();

	if(speed == SPEED::SP_10_MHZ)
		sp_10_mhz();
	else if(speed == SPEED::SP_2_MHZ)
		sp_2_mhz();
	else if(speed == SPEED::SP_50_MHZ)
		sp_50_mhz();
	else if(speed == SPEED::IN_MODE)
		in_mode();
}
