#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>

#include "chess_clock.h"
#include "spi.h"
#include "tmr.h"
#include "max7219.h"

#define true	1
#define false	0

/*
 * This will be set to 1 in the timer1 ISR and cleared after we've done our
 * thing, on the main loop. This is an 8 bit flag and we shouldn't need any
 * protection on accessing it since reading/writing to it it's done in a single
 * clock cycle.
 */
static volatile uint8_t clock_tick;

static uint8_t active_clock = 0;

static volatile uint8_t button_pressed;

static void int0_enable()
{
	GICR |= _BV(INT0);
}

static void int0_disable()
{
	GICR &= ~_BV(INT0);
}

ISR(SPI_STC_vect)
{
}

ISR(TIMER1_OVF_vect)
{
	TCNT1 = TMR1_RESET_VALUE;
	clock_tick = true;
}

ISR(INT0_vect)
{
	/* disable the int0 interrupt */
	int0_disable();

	PORTC ^= _BV(PC1);
	button_pressed = true;
}



int main(void) {
	DDRC = _BV(DDC1);
	int0_enable();

	spi_master_init();

	/* Enable interrupts */
	sei();

	max7219_init();

	chess_clock_init();
	chess_clock_set(0, 90, 0, TMR1_RESET_VALUE);
	chess_clock_set(1, 90, 0, TMR1_RESET_VALUE);
	chess_clock_set_active(active_clock);
	chess_clock_start();

	while (1) {
		if (clock_tick) {
			if (chess_clock_tick() < 0)
				chess_clock_stop();

			clock_tick = false;
		}

		if (button_pressed) {
			_delay_ms(500);
			active_clock ^= 0x1;
			chess_clock_stop();
			chess_clock_set_active(active_clock);
			chess_clock_start();
			button_pressed = false;

			int0_enable();
		}
	}

	return 0;
}
