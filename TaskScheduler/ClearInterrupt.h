/*
 * ClearInterrupt.h
 *
 *  Created on: 16 mars 2014
 *      Author: Jean-François
 */

#ifndef CLEARINTERRUPT_H_
#define CLEARINTERRUPT_H_

#include <Arduino.h>

// Utility class to disable interrupts and restore them at the end
class ClearInterrupt
{
public:
	inline ClearInterrupt():_status(SREG)
	{
		cli();
	}
	inline ~ClearInterrupt()
	{
		SREG = _status;
	}

private:
	const uint8_t _status;
};

#endif /* CLEARINTERRUPT_H_ */
