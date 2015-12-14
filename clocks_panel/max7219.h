#ifndef _MAX7219_H_
#define _MAX7219_H_

#define MAX7219_REG_DIG0		0x1
#define MAX7219_REG_DIG1		0x2
#define MAX7219_REG_DIG2		0x3
#define MAX7219_REG_DIG3		0x4
#define MAX7219_REG_DIG4		0x5
#define MAX7219_REG_DIG5		0x6
#define MAX7219_REG_DIG6		0x7
#define MAX7219_REG_DIG7		0x8
#define MAX7219_REG_DECODE_MODE		0x9
#define MAX7219_REG_INTENSITY		0xA
#define MAX7219_REG_SCAN_LIMIT		0xB
#define MAX7219_REG_SHUTDOWN		0xC
#define MAX7219_REG_TEST		0xF

#define MAX7219_DIGIT_ID_TO_ADDR(id)	(id + 1)

#define MAX7219_STATE_OFF		0x0
#define MAX7219_STATE_ON		0x1

#define MAX7219_DECODE_NONE		0x00
#define MAX7219_DECODE_CODEB_0		0x01
#define MAX7219_DECODE_CODEB_0_3	0x0F
#define MAX7219_DECODE_CODEB_0_7	0xFF

struct max7219_state_t {
	/*
	 * State: 0 - shutdown
	 *	  1 - normal operation mode
	 */
	uint8_t state;
	/*
	 * Decode mode: 0x0 - no decode
	 *		0x1 - code b decode for digit 0 (no decode for 7-1)
	 *		0x2 - code b decode for digits 3-0 (no decode for 7-4)
	 *		0x3 - code b decode for digits 7-0
	 */
	uint8_t decode_mode;
	/*
	 * Display intensity: 0x0 - 1/32 duty cycle
	 *		      ...
	 *		      0xf - 31/32 duty cycle
	 */
	uint8_t intensity;
	/*
	 * Scan limit: 0x0 - display digit 0 only
	 *	       0x1 - display digits 0, 1
	 *	       ...
	 *	       0x7 - display digits 0, 1, 2, 3, 4, 5, 6, 7
	 */
	uint8_t scan_limit;
	/*
	 * Digits value, in BCD code B format
	 */
	uint8_t digit[8];
};

void max7219_init();
void max7219_send(uint8_t reg_addr, uint8_t data);
void max7219_test();
void max7219_switch_state(uint8_t state);
void max7219_decode_mode_set(uint8_t decode_mode);
void max7219_intensity_set(uint8_t intensity);
void max7219_scan_limit_set(uint8_t scan_limit);
void max7219_digit_update(uint8_t digit_id, uint8_t bcd_codeb_val);

#endif
