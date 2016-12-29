/*
 *  Variuous utility functions that didn't make it n their own file
 *
 *  Copyright 2016 - Laurentiu Palcu <lpalcu@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

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

void status_led_on()
{
	PORTB |= _BV(PB0);
}

void status_led_off()
{
	PORTB &= ~_BV(PB0);
}

void host_interrupt_assert()
{
	/* PB1 will be asserted low */
	DDRB |= _BV(DDB1);
	PORTB &= ~_BV(PB1);
}

void host_interrupt_release()
{
	/* PB1 pin will be Hi-Z */
	DDRB &= ~_BV(DDB1);
	PORTB &= ~_BV(PB1);
}
