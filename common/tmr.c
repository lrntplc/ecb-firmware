/*
 *  Atmega8 timer driver
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
#include <avr/interrupt.h>

#include "tmr.h"
#include "util.h"


static struct {
	uint8_t prescaler;
	uint16_t reset_value;
	tmr_expiration_cb cb;
} tmr[3];

ISR(TIMER0_OVF_vect)
{
	if (tmr[TMR_0].reset_value)
		TCNT0 = tmr[TMR_0].reset_value && 0xff;
	tmr[TMR_0].cb();
}

ISR(TIMER1_OVF_vect)
{
	if (tmr[TMR_1].reset_value)
		TCNT1 = tmr[TMR_1].reset_value;
	tmr[TMR_1].cb();
}

ISR(TIMER2_OVF_vect)
{
	if (tmr[TMR_2].reset_value)
		TCNT2 = tmr[TMR_2].reset_value && 0xff;
	tmr[TMR_2].cb();
}

/* Init a normal operation timer: WGM bits set to 0 */
void tmr_init(uint8_t tmr_no, uint8_t prescaler, uint16_t reset_value,
	      tmr_expiration_cb cb)
{
	switch (tmr_no) {
		case TMR_0:
			TIMSK |= _BV(TOIE0); /* enable TMR0 interrupt */

			TCCR0 = 0;
			break;

		case TMR_1:
			TIMSK |= _BV(TOIE1); /* enable TMR1 interrupt */

			TCCR1A = TCCR1B = 0;
			break;

		case TMR_2:
			TIMSK = 1 << TOIE2; /* enable TMR2 interrupt */

			/* get the clock from external 32768Hz crystal */
			ASSR |= _BV(AS2);

			TCCR2 = 0;
			break;

		default:
			return;
	}

	tmr[tmr_no].cb = cb;
	tmr[tmr_no].prescaler = prescaler & TMR_PRESCALER_MASK;
	tmr[tmr_no].reset_value = reset_value;
}

void tmr_start(uint8_t tmr_no, uint16_t tmr_value)
{
	switch (tmr_no) {
		case TMR_0:
			TCNT0 = tmr_value & 0xff;
			TCCR0 |= tmr[tmr_no].prescaler;
			break;

		case TMR_1:
			TCNT1 = tmr_value;
			TCCR1B |= tmr[tmr_no].prescaler;
			break;

		case TMR_2:
			TCNT2 = tmr_value & 0xff;
			TCCR2 |= tmr[tmr_no].prescaler;
			break;
	}
}

uint16_t tmr_stop(uint8_t tmr_no)
{
	switch (tmr_no) {
		case TMR_0:
			TCCR0 &= ~TMR_PRESCALER_MASK;
			return TCNT0 & 0xff;

		case TMR_1:
			TCCR1B &= ~TMR_PRESCALER_MASK;
			return TCNT1;

		case TMR_2:
			TCCR2 &= ~TMR_PRESCALER_MASK;
			return TCNT2 & 0xff;
	}

	return 0;
}

uint8_t tmr_status_get(uint8_t tmr_no)
{
	switch (tmr_no) {
		case TMR_0:
			return !!(TCCR0 & TMR_PRESCALER_MASK);

		case TMR_1:
			return !!(TCCR1B & TMR_PRESCALER_MASK);

		case TMR_2:
			return !!(TCCR2 & TMR_PRESCALER_MASK);
	}

	return 0;
}

