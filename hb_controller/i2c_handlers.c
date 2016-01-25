#include <stdint.h>
#include <string.h>

#include "i2c_slave.h"
#include "i2c_handlers.h"
#include "util.h"

static struct i2c_reg *reg_map = NULL;
static uint8_t max_reg_no = 0;
static struct i2c_reg *current_reg = NULL;

static rmap_changed_cb i2c_handlers_cb = NULL;

#define MAX_REG		(&reg_map[max_reg_no - 1])

static void i2c_new_pkt(uint8_t pkt_type)
{
	if (pkt_type == PKT_TYPE_WR)
		current_reg = NULL;
}

/* Receive data from the bus */
static uint8_t i2c_consume(uint8_t data)
{
	if (current_reg > MAX_REG)
		return 0;

	if (!current_reg) {
		if (data >= max_reg_no)
			return 0;

		current_reg = &reg_map[data];

		goto consume_out;
	}

	if (!current_reg->read_only) {
		current_reg->reg = data;
		current_reg->reg_changed = true;
		i2c_handlers_cb();
	}

	current_reg++;

consume_out:
	return MAX_REG - current_reg + 1;
}

/* Send data to the bus */
static uint8_t i2c_feed(uint8_t *data)
{
	if (!current_reg || current_reg > MAX_REG) {
		*data = 0xff;
		return 0;
	}

	*data = current_reg->reg;

	current_reg++;

	return MAX_REG - current_reg + 1;
}

struct i2c_slave_handlers i2c_handlers = {
	.new_pkt = i2c_new_pkt,
	.consume = i2c_consume,
	.feed = i2c_feed,
};

void i2c_handlers_init(struct i2c_reg *rmap, uint8_t rmap_size,
		       rmap_changed_cb cb)
{
	reg_map = rmap;
	max_reg_no = rmap_size;
	i2c_handlers_cb = cb;

	i2c_slave_init(I2C_ADDRESS, &i2c_handlers);
}
