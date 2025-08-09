#include "Settings.h"
#include "Timer.h"
#if ENA_MODE_W_EEPROM == 0

Timer::Timer()
{
	this->cnt_1Hz = 0;
	this->ena_timer_1Hz = false;
	
	TCCR2B |= (0 << CS22) | (1 << CS21) | (0 << CS20);
	TIMSK2 |= (1 << TOIE2);		//enable interrupt
	sei();
}

ISR(TIMER2_OVF_vect)	// F = 16MHz / (8 * 200) = 10 000 Hz
{
	TIMSK2 &= ~(1 << TOIE2);			//disable interrupt
	TCNT2 = 0xFF - 220;
	if (timer.cnt_1Hz >= 163) {
		timer.ena_timer_1Hz = true;
		timer.cnt_1Hz = 0;
	} else {
		timer.cnt_1Hz++;
	}

	TIMSK2 |= (1 << TOIE2);				//enable interrupt
}


#endif