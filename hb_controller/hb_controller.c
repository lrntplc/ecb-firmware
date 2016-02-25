#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>

#include "chess_clock.h"
#include "spi.h"
#include "max7219.h"
#include "sensors.h"
#include "i2c_handlers.h"
#include "util.h"

#define CMD_START_CLOCK		(1 << 0)
#define CMD_START_SENSORS	(1 << 1)
#define CMD_BLANK_CLOCK		(1 << 2)
#define CMD_MASK		0x07

#define STATUS_CLOCK_EXPIRED	(1 << 0)
#define STATUS_SENSORS_CHANGED	(1 << 1)
#define STATUS_MASK		0x03

#define MAX7219_LED_ROW0	MAX7219_REG_DIG4

enum {
	REG_LED_ROW_0 = 0,
	REG_LED_ROW_1,
	REG_LED_ROW_2,
	REG_LED_ROW_3,
	REG_SENSOR_ROW_0,
	REG_SENSOR_ROW_1,
	REG_SENSOR_ROW_2,
	REG_SENSOR_ROW_3,
	REG_CLOCK_MIN,
	REG_CLOCK_SEC,
	REG_COMMAND,
	REG_STATUS,

	REG_TOTAL_NUMBER
};

static struct i2c_reg reg_map[] = {
							  /* changed by: */
	[REG_LED_ROW_0]		= {.read_only = false, }, /* ISR */
	[REG_LED_ROW_1]		= {.read_only = false, }, /* ISR */
	[REG_LED_ROW_2]		= {.read_only = false, }, /* ISR */
	[REG_LED_ROW_3]		= {.read_only = false, }, /* ISR */
	[REG_SENSOR_ROW_0]	= {.read_only = true, },  /* main loop */
	[REG_SENSOR_ROW_1]	= {.read_only = true, },  /* main loop */
	[REG_SENSOR_ROW_2]	= {.read_only = true, },  /* main loop */
	[REG_SENSOR_ROW_3]	= {.read_only = true, },  /* main loop */
	[REG_CLOCK_MIN]		= {.read_only = false, }, /* ISR and main loop */
	[REG_CLOCK_SEC]		= {.read_only = false, }, /* ISR and main loop */
	[REG_COMMAND]		= {.read_only = false, }, /* ISR */
	[REG_STATUS]		= {.read_only = false, }, /* ISR and main loop */
};

static volatile uint8_t clock_ticked = false;
static volatile uint8_t rmap_changed = false;
static volatile uint8_t sensors_changed = false;

/* The callbacks are called from ISR context */
static void rmap_changed_callback()
{
	rmap_changed = true;
}

static void clock_ticked_callback()
{
	clock_ticked = true;
}

static void sensors_changed_callback()
{
	sensors_changed = true;
}

/* The handlers are called from main loop */
static void hb_ctrl_handle_clock() {
	if (chess_clock_tick() < 0) {
		/* time's up! let the host know! */
		reg_map[REG_STATUS].feed_val |= STATUS_CLOCK_EXPIRED;
		reg_map[REG_STATUS].fv_changed = true;

		rmap_changed = true;

		chess_clock_stop();
	}
}

static void hb_ctrl_handle_sensors() {
	uint8_t sensors[4];
	uint8_t i2c_reg, i;

	sensors_state_get(sensors);

	for (i2c_reg = REG_SENSOR_ROW_0, i = 0;
			i < SENSOR_ROWS; i++, i2c_reg++) {
		if (reg_map[i2c_reg].feed_val != sensors[i]) {
			reg_map[i2c_reg].feed_val = sensors[i];
			reg_map[i2c_reg].fv_changed = true;
		}
	}

	reg_map[REG_STATUS].feed_val |= STATUS_SENSORS_CHANGED;
	reg_map[REG_STATUS].fv_changed = true;

	rmap_changed = true;
}

static void hb_ctrl_reg_cmd_chgd(struct i2c_reg *reg, uint8_t index)
{
	uint8_t reg_changes = (reg->feed_val ^ reg->consume_val) & CMD_MASK;

	if (reg_changes & CMD_START_CLOCK) {
		if (reg->consume_val & CMD_START_CLOCK)
			chess_clock_start();
		else {
			chess_clock_stop();
			chess_clock_get(&reg_map[REG_CLOCK_MIN].feed_val,
					&reg_map[REG_CLOCK_SEC].feed_val);
		}
	}

	if (reg_changes & CMD_START_SENSORS) {
		if (reg->consume_val & CMD_START_SENSORS) {
			uint8_t sensors[4];
			uint8_t i2c_reg, i;

			sensors_scan_start();

			sensors_state_get(sensors);
			for (i2c_reg = REG_SENSOR_ROW_0, i = 0;
				i < SENSOR_ROWS; i++, i2c_reg++)
				reg_map[i2c_reg].feed_val = sensors[i];
		} else
			sensors_scan_stop();
	}

	if (reg_changes & CMD_BLANK_CLOCK) {
		chess_clock_blank_all();

		/* this bit is write only, reading will return 0 */
		reg->consume_val &= ~CMD_BLANK_CLOCK;
	}

	reg_map[index].feed_val = reg->consume_val;
}

static void hb_ctrl_reg_status_chgd(struct i2c_reg *reg, uint8_t index)
{
	uint8_t new_fv = reg->feed_val;

	/*
	 * Clear the status bits remote wanted cleared but make sure we don't
	 * set any bits. Only the device is allowed to set them.
	 */
	if (reg->cv_changed)
		new_fv = (reg->feed_val ^ reg->consume_val) & reg->feed_val;

	/*
	 * If the fv_changed flag is set, it means that the remote didn't read
	 * the status register, yet. Hence, if we release the interrupt line,
	 * there's the possibility of missing events. Let's make sure we leave
	 * any relevant bits active, so the host doesn't miss any events. For
	 * that, we have to check all relevant registers to see if they were
	 * acked by the host.
	 */
	if (reg->fv_changed && !(new_fv & STATUS_SENSORS_CHANGED)) {
		uint8_t i;

		for (i = REG_SENSOR_ROW_0; i <= REG_SENSOR_ROW_3; i++)
			if (reg_map[i].fv_changed)
				new_fv |= STATUS_SENSORS_CHANGED;
	}

	if (new_fv)
		host_interrupt_assert();
	else
		host_interrupt_release();

	reg_map[index].feed_val = new_fv;
}

static void hb_ctrl_reg_leds_chgd(struct i2c_reg *reg, uint8_t index)
{
	uint8_t offset = index - REG_LED_ROW_0;

	max7219_send(MAX7219_LED_ROW0 + offset, reg->consume_val);

	reg_map[index].feed_val = reg->consume_val;
}

static void hb_ctrl_reg_clock_chgd(struct i2c_reg *reg, uint8_t index)
{
	if (!reg->cv_changed)
		return;

	uint8_t min = index == REG_CLOCK_MIN ?
			       reg->consume_val :
			       reg_map[REG_CLOCK_MIN].feed_val;

	uint8_t sec = index == REG_CLOCK_SEC ?
			       reg->consume_val :
			       reg_map[REG_CLOCK_SEC].feed_val;

	chess_clock_set(min, sec);

	reg_map[index].feed_val = reg->consume_val;
}

typedef void (*reg_chgd_handler_t)(struct i2c_reg *, uint8_t);

static const reg_chgd_handler_t reg_chgd_handler[] = {
	[REG_LED_ROW_0]		= hb_ctrl_reg_leds_chgd,
	[REG_LED_ROW_1]		= hb_ctrl_reg_leds_chgd,
	[REG_LED_ROW_2]		= hb_ctrl_reg_leds_chgd,
	[REG_LED_ROW_3]		= hb_ctrl_reg_leds_chgd,
	[REG_SENSOR_ROW_0]	= NULL,
	[REG_SENSOR_ROW_1]	= NULL,
	[REG_SENSOR_ROW_2]	= NULL,
	[REG_SENSOR_ROW_3]	= NULL,
	[REG_CLOCK_MIN]		= hb_ctrl_reg_clock_chgd,
	[REG_CLOCK_SEC]		= hb_ctrl_reg_clock_chgd,
	[REG_COMMAND]		= hb_ctrl_reg_cmd_chgd,
	[REG_STATUS]		= hb_ctrl_reg_status_chgd,
};

static void hb_ctrl_handle_rmap() {
	uint8_t index;

	for (index = 0; index < REG_TOTAL_NUMBER; index++) {
		/* make a copy of the register, to work with */
		struct i2c_reg reg = reg_map[index];

		if ((!reg.fv_changed && !reg.cv_changed) || reg.read_only)
			continue;

		/* ack the remote changes */
		reg_map[index].cv_changed = false;

		if (reg_chgd_handler[index])
			reg_chgd_handler[index](&reg, index);
	}
}

static void hb_ctrl_main_loop()
{
	while (1) {
		if (clock_ticked) {
			clock_ticked = false;

			hb_ctrl_handle_clock();
		}

		if (rmap_changed) {
			rmap_changed = false;

			hb_ctrl_handle_rmap();
		}

		if (sensors_changed) {
			sensors_changed = false;

			hb_ctrl_handle_sensors();
		}
	}
}

int main(void) {
	status_led_init();

	host_interrupt_release();

	spi_master_init();

	i2c_handlers_init(reg_map, ARRAY_SIZE(reg_map), rmap_changed_callback);

	/* Enable interrupts */
	sei();

	max7219_init();

	chess_clock_init(clock_ticked_callback);

	sensors_init(sensors_changed_callback);

	hb_ctrl_main_loop();

	return 0;
}
