/*
 *  Atmega8 timer driver header
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

#ifndef _TMR_H_
#define _TMR_H_

enum {
	TMR_0,
	TMR_1,
	TMR_2
};

enum {
	TMR_STATUS_STOPPED,
	TMR_STATUS_RUNNING,
};

typedef void (*tmr_expiration_cb)();

/* TMR0 and TMR1 prescaler values */
#define TMR01_PRESCALER_STOPPED	0 /* No clock source (Timer/Counter stopped) */
#define TMR01_PRESCALER_NONE	1 /* no prescaling */
#define TMR01_PRESCALER_8	2 /* clkT2S/8 */
#define TMR01_PRESCALER_64	3 /* clkT2S/64 */
#define TMR01_PRESCALER_256	4 /* clkT2S/256 */
#define TMR01_PRESCALER_1024	5 /* clkT2S/1024 */
#define TMR01_PRESCALER_T1_F	6 /* external clock on T1 pin, falling edge */
#define TMR01_PRESCALER_T1_R	7 /* external clock on T1 pin, rising edge */

/* TMR2 prescaler values */
#define TMR2_PRESCALER_STOPPED	0 /* No clock source (Timer/Counter stopped) */
#define TMR2_PRESCALER_NONE	1 /* no prescaling */
#define TMR2_PRESCALER_8	2 /* clkT2S/8 */
#define TMR2_PRESCALER_32	3 /* clkT2S/32 */
#define TMR2_PRESCALER_64	4 /* clkT2S/64 */
#define TMR2_PRESCALER_128	5 /* clkT2S/128 */
#define TMR2_PRESCALER_256	6 /* clkT2S/256 */
#define TMR2_PRESCALER_1024	7 /* clkT2S/1024 */

#define TMR_PRESCALER_MASK	0x7

void tmr_init(uint8_t tmr_no, uint8_t prescaler, uint16_t reset_value,
	      tmr_expiration_cb cb);
void tmr_start(uint8_t tmr_no, uint16_t tmr_value);
uint16_t tmr_stop(uint8_t tmr_no);
uint8_t tmr_status_get(uint8_t tmr_no);

#endif
