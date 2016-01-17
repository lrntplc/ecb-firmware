#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>

#include "chess_clock.h"
#include "spi.h"
#include "tmr.h"
#include "max7219.h"
#include "i2c_handlers.h"
#include "util.h"

static struct i2c_reg reg_map[] = {
	{.read_only = false, }, /* led row 1 */
	{.read_only = false, }, /* led row 2 */
	{.read_only = false, }, /* led row 2 */
	{.read_only = false, }, /* led row 4 */
	{.read_only = true, },  /* sensor row 1 */
	{.read_only = true, },  /* sensor row 2 */
	{.read_only = true, },  /* sensor row 3 */
	{.read_only = true, },  /* sensor row 4 */
};

/*
 * This will be set to 1 in the timer1 ISR and cleared after we've done our
 * thing, on the main loop. This is an 8 bit flag and we shouldn't need any
 * protection on accessing it since reading/writing to it it's done in a single
 * clock cycle.
 */
static volatile uint8_t clock_tick;

static uint8_t active_clock = 0;

ISR(TIMER1_OVF_vect)
{
	TCNT1 = TMR1_RESET_VALUE;
	clock_tick = true;
}

void rmap_changed()
{
}

static void hb_ctrl_main_loop()
{
	while (1) {
		if (clock_tick) {
			if (chess_clock_tick() < 0)
				chess_clock_stop();

			clock_tick = false;
		}
	}
}

int main(void) {
//[lp]	spi_master_init();
	i2c_handlers_init(reg_map, ARRAY_SIZE(reg_map), rmap_changed);

	/* Enable interrupts */
	sei();

//[lp]	max7219_init();
//[lp]	max7219_test();
	while(1)
		;

	chess_clock_init();
	chess_clock_set(0, 90, 0, TMR1_RESET_VALUE);
	chess_clock_set(1, 90, 0, TMR1_RESET_VALUE);
	chess_clock_set_active(active_clock);
	chess_clock_start();

	hb_ctrl_main_loop();

	return 0;
}
