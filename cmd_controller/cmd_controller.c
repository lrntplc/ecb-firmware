/*
 *  Command panel implementation
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
#include <string.h>
#include <util/delay.h>

#include "util.h"
#include "i2c_handlers.h"

enum {
	REG_LED = 0,
	REG_BTN,
	REG_CMD,

	REG_TOTAL_NUMBER
};

#define BTN_MODE		(1 << 2)
#define BTN_OPPONENT_LEVEL	(1 << 3)
#define BTN_OPPONENT_COLOR	(1 << 4)
#define BTN_GAME_TIME		(1 << 5)
#define BTN_GAME_START		(1 << 6)

#define BTN_MASK		0x7c

					 /* used for promotion indication too */
#define LED_GAME_START		(1 << 0) /* Bishop */
#define LED_OPPONENT_COLOR	(1 << 1) /* Knight */
#define LED_OPPONENT_LEVEL0	(1 << 2) /* Rook */
#define LED_OPPONENT_LEVEL1	(1 << 3)
#define LED_OPPONENT_LEVEL2	(1 << 4)
#define LED_MODE		(1 << 5) /* Queen */
#define LED_WIFI_ON		(1 << 6)
#define LED_BT_ON		(1 << 7)

#define CMD_START_BOOTLOADER	(1 << 3) /* start the bootloader */
#define CMD_MASK		0x0f

static void (*start_bootloader)(void) __attribute__((noreturn)) = (void*) 0x1c00;

static struct i2c_reg reg_map[] = {
							  /* changed by: */
	[REG_LED]		= {.read_only = false, }, /* ISR */
	[REG_BTN]		= {.read_only = false, }, /* ISR and main loop */
	[REG_CMD]		= {.read_only = false, }, /* ISR */
};

static volatile uint8_t rmap_changed = false;
static volatile uint8_t buttons_changed = false;

/* The callbacks are called from ISR context */
static void rmap_changed_callback()
{
	rmap_changed = true;
}

static void led_tmr_init()
{
	TCCR1A = 0;
	TCCR1B = _BV(WGM12);

	/* enable Output Compare A&B Match interrupts */
	TIMSK |= _BV(OCIE1A) | _BV(OCIE1B);

	/* this results in 240Hz frequency: 8000000 / 64 / 520 */
	OCR1A = 312;

	/* set duty cycle to 10% */
	OCR1B = 5;
}

static uint8_t led_tmr_running = false;

static void led_tmr_start()
{
	TCCR1B |= _BV(CS11) | _BV(CS10); /* set prescaler to 64 */

	led_tmr_running = true;
}

static void led_tmr_stop()
{
	TCCR1B &= ~0x7;

	led_tmr_running = false;
}

static volatile uint8_t buttons_stable_state = 0;

static inline void btn_check()
{
	static uint8_t cnt_high = 0xff, cnt_low = 0xff;
	static uint8_t btn_current_state = 0;

	uint8_t btns_changed = btn_current_state ^ ((~PINB & BTN_MASK) >> 2);

	cnt_low = ~(cnt_low & btns_changed);
	cnt_high = cnt_low ^ (cnt_high & btns_changed);

	btns_changed &= cnt_low & cnt_high;

	btn_current_state ^= btns_changed;

	if (buttons_stable_state ^ (btn_current_state & btns_changed)) {
		buttons_stable_state |= btn_current_state & btns_changed;
		buttons_changed = true;
	}
}

ISR(TIMER1_COMPA_vect)
{
	static uint8_t current_led = 0;
	static uint8_t check_buttons = false;

	PORTD = reg_map[REG_LED].feed_val & (1 << current_led);

	current_led = (current_led + 1) & 0x7;

	OCR1B = 5;

	if (check_buttons) {
		btn_check();
		check_buttons = false;
	} else
		check_buttons = true;

}

ISR(TIMER1_COMPB_vect)
{
	PORTD = 0;
}

/* The handlers are called from main loop */
static void cmd_ctrl_reg_leds_chgd(struct i2c_reg *reg, uint8_t index)
{
	reg_map[index].feed_val = reg->consume_val;
}

static void cmd_ctrl_reg_btn_chgd(struct i2c_reg *reg, uint8_t index)
{
	reg->feed_val ^= reg->consume_val & reg->feed_val;

	if (!reg->feed_val)
		host_interrupt_release();

	buttons_stable_state = reg->feed_val;

	reg_map[index].feed_val = reg->feed_val;
}

static void cmd_ctrl_reg_cmd_chgd(struct i2c_reg *reg, uint8_t index)
{
	uint8_t reg_changes = (reg->feed_val ^ reg->consume_val) & CMD_MASK;

	if (reg_changes & CMD_START_BOOTLOADER) {
		/* stop led timer */
		TCCR1B &= ~(_BV(CS11) | _BV(CS10));

		/* switch off all leds */
		PORTD = 0;

		host_interrupt_release();

		cli();

		start_bootloader();
	}

	reg_map[index].feed_val = reg->consume_val;
}

typedef void (*reg_chgd_handler_t)(struct i2c_reg *, uint8_t);

static const reg_chgd_handler_t reg_chgd_handler[] = {
	[REG_LED]		= cmd_ctrl_reg_leds_chgd,
	[REG_BTN]		= cmd_ctrl_reg_btn_chgd,
	[REG_CMD]		= cmd_ctrl_reg_cmd_chgd,
};

static void cmd_ctrl_handle_rmap() {
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

static void cmd_ctrl_main_loop()
{
	led_tmr_start();

	while (1) {
		if (rmap_changed) {
			rmap_changed = false;

			cmd_ctrl_handle_rmap();
		}

		if (buttons_changed) {
			buttons_changed = false;

			reg_map[REG_BTN].feed_val = buttons_stable_state;

			host_interrupt_assert();
		}
	}
}

int main(void) {
	DDRD = 0xff; /* all outputs */

	DDRB &= ~BTN_MASK; /* btn pins inputs */
	PORTB |= BTN_MASK;  /* btn pull-up on */

	host_interrupt_release();

	i2c_handlers_init(reg_map, ARRAY_SIZE(reg_map), rmap_changed_callback);

	led_tmr_init();

	/* Enable interrupts */
	sei();

	cmd_ctrl_main_loop();

	return 0;
}
