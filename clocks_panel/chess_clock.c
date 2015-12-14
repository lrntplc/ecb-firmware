#include <avr/io.h>

#include "tmr.h"
#include "chess_clock.h"
#include "max7219.h"

static uint8_t active_clock = 0;

static struct chess_clock_t clocks[2] = {
	/* first chess clock */
	{
		.max7219_digits_min = {0, 1},
		.max7219_digits_sec = {2, 3},
	},
	/* second chess clock */
	{
		.max7219_digits_min = {4, 5},
		.max7219_digits_sec = {6, 7},
	}
};

void chess_clock_init()
{
	tmr1_init();
}

void chess_clock_start()
{
	tmr1_start(clocks[active_clock].tmr1);
}

void chess_clock_stop()
{
	tmr1_stop();
	clocks[active_clock].tmr1 = TCNT1;
}

void chess_clock_set_active(uint8_t clk_id)
{
	active_clock = clk_id;
}

/* Update the actual digits on the display */
static void chess_clock_update(uint8_t id)
{
	max7219_digit_update(clocks[id].max7219_digits_min[0],
			     clocks[id].min / 10);
	max7219_digit_update(clocks[id].max7219_digits_min[1],
			     clocks[id].min % 10);

	max7219_digit_update(clocks[id].max7219_digits_sec[0],
			     clocks[id].sec / 10);
	max7219_digit_update(clocks[id].max7219_digits_sec[1],
			     clocks[id].sec % 10);
}

/* Setup the clock */
void chess_clock_set(uint8_t id, uint8_t min, uint8_t sec, uint16_t tmr_value)
{
	clocks[id].min = min;
	clocks[id].sec = sec;
	clocks[id].tmr1 = tmr_value;

	chess_clock_update(id);
}

/* Decrement the active clock */
int8_t chess_clock_tick()
{
	uint8_t id = active_clock;

	if (clocks[id].min > 0 && clocks[id].sec == 0) {
		clocks[id].min--;
		clocks[id].sec = 59;
	} else
		clocks[id].sec--;

	chess_clock_update(id);

	if (clocks[id].min == 0 && clocks[id].sec == 0)
		return -1; /* time's up */

	return 0;
}
