#ifndef _I2C_HANDLERS_H_
#define _I2C_HANDLERS_H_

struct i2c_reg {
	uint8_t reg;
	uint8_t read_only : 1;
	uint8_t reg_changed : 1;
	uint8_t reserved : 6;
};

typedef void (*rmap_changed_cb)();

void i2c_handlers_init(struct i2c_reg *rmap, uint8_t rmap_size,
		       rmap_changed_cb cb);
#endif
