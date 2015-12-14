#include <avr/io.h>

#include "tmr.h"

void tmr1_init()
{
	TIMSK = 1 << TOIE1;
	/* choose normal operation: WGM bits set to 0 */
	TCCR1A = TCCR1B = 0;
}

void tmr1_start(uint16_t tmr_value)
{
	TCNT1 = tmr_value;
	TCCR1B |= 0x4 << CS10;
}

void tmr1_stop()
{
	TCCR1B &= ~TMR1_CLOCK_SELECT_MASK;
}


