#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>

#include "chess_clock.h"
#include "spi.h"
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
	SENSOR_ROW_3,
	CLOCK_MIN,
	CLOCK_SEC,
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
	[CLOCK_MIN]	= {.read_only = false, },
	[CLOCK_SEC]	= {.read_only = false, },
};

static volatile uint8_t clock_ticked = false;
static volatile uint8_t reg_map_changed = false;

/* called from ISR context */
static void rmap_changed()
{
	reg_map_changed = true;
}

static void clock_ticked_callback()
{
	clock_ticked = true;
}

static void hb_ctrl_main_loop()
{
	while (1) {
		if (clock_ticked) {
			if (chess_clock_tick() < 0)
				chess_clock_stop();

			clock_ticked = false;

			status_led_toggle();
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
	status_led_init();

	spi_master_init();

	i2c_handlers_init(reg_map, ARRAY_SIZE(reg_map), rmap_changed);

	/* Enable interrupts */
	sei();

	max7219_init();

	chess_clock_init(clock_ticked_callback);
	chess_clock_set(90, 0);
	chess_clock_start();

	hb_ctrl_main_loop();

	return 0;
}
