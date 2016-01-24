#include <avr/io.h>
#include <util/delay.h>

#include "util.h"

void status_led_init()
{
	DDRB |= _BV(DDB0);
}

void status_led_blink()
{
	while (true) {
		PORTB ^= _BV(PB0);
		_delay_ms(500);
	}
}

void status_led_toggle()
{
	PORTB ^= _BV(PB0);
}


