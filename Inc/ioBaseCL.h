/*
 * ioBaseCL.h
 *
 *  Created on: Nov 16, 2022
 *      Author: Marcin
 */

#ifndef IOBASECL_H_
#define IOBASECL_H_

#include <stdint.h>

// Global macro definitions.
#define pSTATUS_ERR (0)
#define pSTATUS_SET (1)
#define pSTATUS_ON  (2)

class pIO
{
public:
	pIO() {}
  // Common read/write methods.
	virtual unsigned read(void)	{return 0;}
	virtual void write(unsigned dat){}
  // Common peripheral control methods.
	virtual void clock_en(void);

	virtual void reset(void);

	virtual void disable(void);

	// Getters/Setters.
	virtual int get_status(void) { return status; }
protected:
  // Expected peripheral status.
	int status = pSTATUS_ERR;
  // Enable/disable/reset register definitions.
	volatile uint32_t *enable_reg = 0;
	volatile uint32_t *reset_reg  = 0;
	uint32_t       enable_bit = 0;
	uint32_t       reset_bit  = 0;
private:
};

#endif /* IOBASECL_H_ */
