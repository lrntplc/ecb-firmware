/*
 *  Atmega8 I2C slave driver.
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
#include <util/twi.h>
#include <stddef.h>

#include "i2c_slave.h"

typedef uint8_t (*i2c_state_t)(uint8_t);

static void i2c_slave_state_set(i2c_state_t new_state);
static uint8_t i2c_slave_sm_idle(uint8_t event);
static uint8_t i2c_slave_sm_transmit(uint8_t event);
static uint8_t i2c_slave_sm_receive(uint8_t event);

static struct i2c_slave_handlers *i2c_handlers;

static uint8_t i2c_slave_sm_idle(uint8_t event)
{
	uint8_t tx_data;
	uint8_t twcr = TWCR;

	switch(event) {
		case TW_SR_SLA_ACK:
		case TW_SR_ARB_LOST_SLA_ACK:
		case TW_SR_GCALL_ACK:
		case TW_SR_ARB_LOST_GCALL_ACK:
			i2c_handlers->new_pkt(PKT_TYPE_WR);
			i2c_slave_state_set(i2c_slave_sm_receive);
			break;

		case TW_ST_SLA_ACK:
		case TW_ST_ARB_LOST_SLA_ACK:
			i2c_handlers->new_pkt(PKT_TYPE_RD);

			if (i2c_handlers->feed(&tx_data))
				twcr |= _BV(TWEA);
			else
				twcr &= ~_BV(TWEA);

			TWDR = tx_data;

			i2c_slave_state_set(i2c_slave_sm_transmit);

			break;

		default:
			break;
	}

	return twcr;
}

static uint8_t i2c_slave_sm_transmit(uint8_t event)
{
	uint8_t tx_data;
	uint8_t twcr = TWCR;

	switch(event) {
		case TW_ST_DATA_ACK:
			if (i2c_handlers->feed(&tx_data))
				twcr |= _BV(TWEA);
			else
				twcr &= ~_BV(TWEA);

			TWDR = tx_data;
			break;

		case TW_ST_DATA_NACK:
		case TW_ST_LAST_DATA:
			twcr |= _BV(TWEA);
			i2c_slave_state_set(i2c_slave_sm_idle);
			break;

		default:
			break;
	}

	return twcr;
}

static uint8_t i2c_slave_sm_receive(uint8_t event)
{
	uint8_t twcr = TWCR;

	switch(event) {
		case TW_SR_DATA_ACK:
		case TW_SR_GCALL_DATA_ACK:
		case TW_SR_DATA_NACK:
		case TW_SR_GCALL_DATA_NACK:
			if (i2c_handlers->consume(TWDR))
				twcr |= _BV(TWEA);
			else
				twcr &= ~_BV(TWEA);
			break;

		case TW_SR_STOP:
			twcr |= _BV(TWEA);
			i2c_slave_state_set(i2c_slave_sm_idle);
			break;

		default:
			break;
	}

	return twcr;
}

static i2c_state_t i2c_state = i2c_slave_sm_idle;

static void i2c_slave_state_set(i2c_state_t new_state)
{
	i2c_state = new_state;
}

ISR(TWI_vect)
{
	/* handle the interrupt and set the TWCR accordingly */
	TWCR = i2c_state(TW_STATUS);
}

/* Init the TWI controller in slave mode. */
void i2c_slave_init(uint8_t address, struct i2c_slave_handlers *handlers)
{
	if (handlers)
		i2c_handlers = handlers;

	TWAR = address << TWA0;

	TWCR = _BV(TWEA) | /* enable acknowledge bit */
	       _BV(TWIE) | /* enable TWI interrupt */
	       _BV(TWEN);  /* enable TWI */
}
