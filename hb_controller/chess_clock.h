#ifndef _CHESS_CLOCK_H_
#define _CHESS_CLOCK_H_

typedef void (*chess_clock_tick_cb)();

struct chess_clock_t {
	uint8_t min;
	uint8_t sec;

	/*
	 * This will hold the tmr counter value before switching the timer on.
	 */
	uint16_t tmr_value;

	/* max7219 digits used */
	uint8_t max7219_digits_min[2];
	uint8_t max7219_digits_sec[2];

	chess_clock_tick_cb cb;
};


void chess_clock_init(chess_clock_tick_cb cb);
void chess_clock_start();
void chess_clock_stop();
void chess_clock_set(uint8_t min, uint8_t sec);
int8_t chess_clock_tick();

#endif
