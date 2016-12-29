/*
 *  Chess clock module
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

#include "tmr.h"
#include "chess_clock.h"
#include "max7219.h"

static struct chess_clock_t chess_clock = {
	.tmr_value = 0,
	.max7219_digits_min = {0, 1},
	.max7219_digits_sec = {2, 3},
};

static void tmr_expiration_callback()
{
	chess_clock.cb();
}

static void chess_clock_blank_digit(uint8_t digit)
{
	max7219_send(MAX7219_DIGIT_ID_TO_ADDR(digit), 0x0f);
}

static void chess_clock_blank_leading_digit()
{
	chess_clock_blank_digit(chess_clock.max7219_digits_min[0]);
}

void chess_clock_blank_all()
{
	chess_clock_blank_digit(chess_clock.max7219_digits_min[0]);
	chess_clock_blank_digit(chess_clock.max7219_digits_min[1]);
	chess_clock_blank_digit(chess_clock.max7219_digits_sec[0]);
	chess_clock_blank_digit(chess_clock.max7219_digits_sec[1]);
}

void chess_clock_init(chess_clock_tick_cb cb)
{
	chess_clock.cb = cb;
	tmr_init(TMR_2, TMR2_PRESCALER_128, 0, tmr_expiration_callback);
}

void chess_clock_start()
{
	tmr_start(TMR_2, chess_clock.tmr_value);
}

void chess_clock_stop()
{
	chess_clock.tmr_value = tmr_stop(TMR_2);
}

static void chess_clock_show_dashes()
{
	max7219_send(MAX7219_DIGIT_ID_TO_ADDR(chess_clock.max7219_digits_min[0]), 0xa);
	max7219_send(MAX7219_DIGIT_ID_TO_ADDR(chess_clock.max7219_digits_min[1]), 0xa);
	max7219_send(MAX7219_DIGIT_ID_TO_ADDR(chess_clock.max7219_digits_sec[0]), 0xa);
	max7219_send(MAX7219_DIGIT_ID_TO_ADDR(chess_clock.max7219_digits_sec[1]), 0xa);
}

/* Update the actual digits on the display */
static void chess_clock_update()
{
	if (!chess_clock.min && !chess_clock.sec) {
		chess_clock_show_dashes();
		return;
	}

	if (chess_clock.min / 10)
		max7219_digit_update(chess_clock.max7219_digits_min[0],
				     chess_clock.min / 10);
	else
		chess_clock_blank_leading_digit();

	max7219_digit_update(chess_clock.max7219_digits_min[1],
			     chess_clock.min % 10);

	max7219_digit_update(chess_clock.max7219_digits_sec[0],
			     chess_clock.sec / 10);
	max7219_digit_update(chess_clock.max7219_digits_sec[1],
			     chess_clock.sec % 10);
}

/* Setup the clock */
void chess_clock_set(uint8_t min, uint8_t sec)
{
	chess_clock.min = min;
	chess_clock.sec = sec;

	chess_clock.tmr_value = 0;

	chess_clock_update();
}

void chess_clock_get(uint8_t *min, uint8_t *sec)
{
	*min = chess_clock.min;
	*sec = chess_clock.sec;
}

/* Decrement the clock */
int8_t chess_clock_tick()
{
	if (chess_clock.min > 0 && chess_clock.sec == 0) {
		chess_clock.min--;
		chess_clock.sec = 59;
	} else
		chess_clock.sec--;

	chess_clock_update();

	if (chess_clock.min == 0 && chess_clock.sec == 0)
		return -1; /* time's up */

	return 0;
}
