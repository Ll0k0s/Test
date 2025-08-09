#ifndef __TIMER_H__
#define __TIMER_H__

#include <avr/interrupt.h>
#define TIME_AUTO_OFF_OUTS				600

class Timer
{
	private:
	public:
	Timer();

	uint16_t cnt_1Hz;
	bool ena_timer_1Hz;
};

extern Timer timer;

#endif //__TIMER_H__