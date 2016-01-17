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

enum {
	LED_ROW_0 = 0,
	LED_ROW_1,
	LED_ROW_2,
	LED_ROW_3,
	SENSOR_ROW_0,
	SENSOR_ROW_1,
	SENSOR_ROW_2,
	SENSOR_ROW_3
};

static struct i2c_reg reg_map[] = {
	[LED_ROW_0]	= {.read_only = false, },
	[LED_ROW_1]	= {.read_only = false, },
	[LED_ROW_2]	= {.read_only = false, },
	[LED_ROW_3]	= {.read_only = false, },
	[SENSOR_ROW_0]	= {.read_only = true, },
	[SENSOR_ROW_1]	= {.read_only = true, },
	[SENSOR_ROW_2]	= {.read_only = true, },
	[SENSOR_ROW_3]	= {.read_only = true, },
};

/*
 * This will be set to 1 in the timer1 ISR and cleared after we've done our
 * thing, on the main loop. This is an 8 bit flag and we shouldn't need any
 * protection on accessing it since reading/writing to it it's done in a single
 * clock cycle.
 */
static volatile uint8_t clock_tick;
static volatile uint8_t reg_map_changed = false;

static uint8_t active_clock = 0;

ISR(TIMER1_OVF_vect)
{
	TCNT1 = TMR1_RESET_VALUE;
	clock_tick = true;
}

/* called from ISR context */
void rmap_changed()
{
	reg_map_changed = true;
	PORTB |= _BV(PB0);
}

static void hb_ctrl_main_loop()
{
	while (1) {
		if (clock_tick) {
			if (chess_clock_tick() < 0)
				chess_clock_stop();

			clock_tick = false;
		}

		if (reg_map_changed) {
			int row;

			for (row = LED_ROW_0; row < LED_ROW_3; row++) {
				if (reg_map[row].reg_changed) {
					/* TODO */
				}
			}
		}
	}
}

int main(void) {
//[lp]	spi_master_init();
	DDRB |= _BV(DDB0);
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
