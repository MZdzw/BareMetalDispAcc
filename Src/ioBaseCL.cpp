/*
 * ioBaseCL.cpp
 *
 *  Created on: Nov 16, 2022
 *      Author: Marcin
 */
#include "ioBaseCL.h"


void pIO::clock_en(void)
{
	if (status == pSTATUS_ERR) { return; }
	*enable_reg |= enable_bit;
	status = pSTATUS_ON;
}
void pIO::reset(void)
{
	if (status == pSTATUS_ERR) { return; }
	*reset_reg |= reset_bit;
	*reset_reg &= ~(reset_bit);
}

void pIO::disable(void)
{
	if (status == pSTATUS_ERR) { return; }
	*enable_reg &= ~(enable_bit);
	status = pSTATUS_SET;
}



