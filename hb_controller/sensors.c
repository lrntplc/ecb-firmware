/*
 *  Sensors scanning implementation.
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

#include "sensors.h"
#include "tmr.h"
#include "util.h"

static uint8_t sensors[4];
static sensors_changed_cb sensors_cb;
static uint8_t current_row = 0;

/*
 * The reset value is chosen to have an expiration rate of 100Hz, in order
 * to check each row 25 times per second.
 *
 * reset_value = 2^16 - Fosc / prescaler / Fexp =
 *             = 2^16 - 8000000 / 8 / 100 = 55536
 */
#define TMR1_RESET_VALUE	55536

static void timer_cb()
{
	static uint8_t row_state;
	static uint8_t debouncing_cnt = 0;

	status_led_toggle();

	if (debouncing_cnt) {
		uint8_t debounced_state = ~PIND;

		if (row_state != debounced_state) {
			debouncing_cnt = 0;
			goto next_row;
		}

		if (--debouncing_cnt)
			return;

		/* we have a steady new state, signal it */
		sensors[current_row] = row_state;

		sensors_cb();

		goto next_row;
	}

	row_state = ~PIND;

	if (sensors[current_row] == row_state) /* nothing changed */
		goto next_row;

	/* activate debouncing (i.e. check the same row 4 times) */
	debouncing_cnt = 4;

	return;

next_row:
	PORTC |= _BV(current_row);
	current_row = (current_row + 1) & 0x3;
	PORTC &= ~_BV(current_row);
}

void sensors_init(sensors_changed_cb cb)
{
	sensors_cb = cb;

	/* configure the column (a to h) pins */
	DDRD = 0; /* configure all PDx pins as inputs */
	PORTD = 0xff; /* activate all pull-up on PDx pins */

	/* configure the row pins */
	DDRC |= _BV(DDC0) | _BV(DDC1) | _BV(DDC2) | _BV(DDC3); /* outputs */
	PORTC |= _BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3); /* set as high */

	tmr_init(TMR_1, TMR01_PRESCALER_8, TMR1_RESET_VALUE, timer_cb);
}

void sensors_scan_start()
{
	uint8_t i;

	/* initial scan of all rows */
	for (i = 0; i < SENSOR_ROWS; i++) {
		PORTC &= ~_BV(i);

		_delay_us(100);

		sensors[i] = ~PIND;

		PORTC |= _BV(i);

	}

	current_row = 0;
	PORTC &= ~_BV(current_row);
	tmr_start(TMR_1, TMR1_RESET_VALUE);
}

void sensors_scan_stop()
{
	tmr_stop(TMR_1);

	status_led_off();
}

void sensors_state_get(uint8_t *sensors_data)
{
	uint8_t i;

	for (i = 0; i < SENSOR_ROWS; i++)
		sensors_data[i] = sensors[i];
}
