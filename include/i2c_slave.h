/*
 *  AVR I2C slave driver.
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

#ifndef _I2C_SLAVE_H_
#define _I2C_SLAVE_H_

enum {
	PKT_TYPE_RD,
	PKT_TYPE_WR
};

struct i2c_slave_handlers {
	/* Let the consumer know that a new SLA-R/W has been received. */
	void (*new_pkt)(uint8_t pkt_type);

	/*
	 * Consume the data from bus. Returns the number of bytes that can be
	 * consumed in burst mode.
	 */
	uint8_t (*consume)(uint8_t data);

	/*
	 * Feed the bus. Returns the number of bytes that can be fed in burst
	 * mode. Also, data should contain the byte to be sent on the bus.
	 */
	uint8_t (*feed)(uint8_t *data);
};

void i2c_slave_init(uint8_t address, struct i2c_slave_handlers *handlers);

#endif /* _I2C_SLAVE_H_ */
