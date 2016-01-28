#ifndef _I2C_HANDLERS_H_
#define _I2C_HANDLERS_H_

struct i2c_reg {
	uint8_t feed_val;    /* The value we send to remote. Set by device. */
	uint8_t consume_val; /* The value received from remote. */
	uint8_t read_only : 1; /* Register is RO or not. */
	uint8_t cv_changed : 1;  /* Remote changed the register. Cleared after
				    device reads consume_val. */
	uint8_t fv_changed : 1;  /* Device changed the register. Cleared after
				    feed_val is sent to remote. */
	uint8_t reserved : 5;
};

typedef void (*rmap_changed_cb)();

void i2c_handlers_init(struct i2c_reg *rmap, uint8_t rmap_size,
		       rmap_changed_cb cb);
#endif
