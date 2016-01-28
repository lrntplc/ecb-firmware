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

void host_interrupt_assert()
{
	/* PB1 will be asserted low */
	DDRB |= _BV(DDB0);
	PORTB |= _BV(PB1);
}

void host_interrupt_release()
{
	/* PB1 pin will be Hi-Z */
	DDRB &= ~_BV(DDB1);
	PORTB &= ~_BV(PB1);
}
