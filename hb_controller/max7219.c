#include <stdint.h>
#include <string.h>

#include "spi.h"
#include "max7219.h"

static struct max7219_state_t max7219;

void max7219_send(uint8_t reg_addr, uint8_t data)
{
	spi_master_cs_set();
	spi_master_send(reg_addr);
	spi_master_send(data);
	spi_master_cs_clear();
}

/* This will turn on all LEDs. */
void max7219_test()
{
	max7219_send(MAX7219_REG_TEST, 1);
}

void max7219_switch_state(uint8_t state)
{
	max7219_send(MAX7219_REG_SHUTDOWN, state);
	max7219.state = state;
}

void max7219_decode_mode_set(uint8_t decode_mode)
{
	max7219_send(MAX7219_REG_DECODE_MODE, decode_mode);
	max7219.decode_mode = decode_mode;
}

void max7219_intensity_set(uint8_t intensity)
{
	max7219_send(MAX7219_REG_INTENSITY, intensity);
	max7219.intensity = intensity;
}

void max7219_scan_limit_set(uint8_t scan_limit)
{
	max7219_send(MAX7219_REG_SCAN_LIMIT, scan_limit);
	max7219.scan_limit = scan_limit;
}

void max7219_digit_update(uint8_t digit_id, uint8_t bcd_codeb_val)
{
	/* switch on the column too */
	if (digit_id == 0 || digit_id == 1 || digit_id == 4 || digit_id == 5)
		bcd_codeb_val ^= 1 << 7;

	max7219_send(MAX7219_DIGIT_ID_TO_ADDR(digit_id), bcd_codeb_val);
}

static uint8_t max7219_blank_values[] = {0x0f, 0x0f, 0x0f, 0x0f, 0, 0, 0, 0};

static void max7219_blank_all() {
	int i;

	for (i = 0; i < sizeof(max7219_blank_values); i++)
		max7219_send(MAX7219_REG_DIG0 + i, max7219_blank_values[i]);
}

void max7219_init()
{
	max7219_switch_state(MAX7219_STATE_OFF);

	memset(&max7219, 0, sizeof(max7219)); /* initialize max7219 structure */

	max7219_decode_mode_set(MAX7219_DECODE_CODEB_0_3); /* code B for 0-3 */
	max7219_scan_limit_set(7); /* scan all digits */
	max7219_intensity_set(0x8); /* set digit intensity to half */

	/* blank all digits */
	max7219_blank_all();

	/* switch on */
	max7219_switch_state(MAX7219_STATE_ON);
}

