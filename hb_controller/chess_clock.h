#ifndef _CHESS_CLOCK_H_
#define _CHESS_CLOCK_H_

struct chess_clock_t {
	uint8_t min;
	uint8_t sec;

	/*
	 * This will hold the tmr1 counter value before switching the timer on.
	 */
	uint16_t tmr1;

	/* max7219 digits used */
	uint8_t max7219_digits_min[2];
	uint8_t max7219_digits_sec[2];
};

void chess_clock_init();
void chess_clock_set_active(uint8_t clock_id);
void chess_clock_start();
void chess_clock_stop();
void chess_clock_set(uint8_t clock_id, uint8_t min,
		     uint8_t sec, uint16_t tmr_value);
int8_t chess_clock_tick();

#endif
