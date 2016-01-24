#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "spi.h"

ISR(SPI_STC_vect)
{
}

void spi_master_init()
{

	/* Set MOSI, SCK and SS as Output */
	DDRB |= (1 << PB5) | (1 << PB3) | (1 << PB2);

	/*
	 * Enable SPI, Set as Master
	 * Prescaler: Fosc/4, Enable Interrupts
	 * The MOSI, SCK pins are as per ATMega8
	 */
	SPCR = (1 << SPE) | (1 << MSTR) | ( 1 << SPIE );
}

void spi_master_cs_set()
{
	PORTB &= ~_BV(PB2);

	_delay_us(1);
}

void spi_master_cs_clear()
{
	_delay_us(1);

	PORTB |= _BV(PB2);
}

void spi_master_send(uint8_t data)
{
	SPDR = data;

	while (!(SPSR & (1 << SPIF)))
		;
}
