/*
 *  Utility header
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

#ifndef _UTIL_H_
#define _UTIL_H_

#define true	1
#define false	0

#define ARRAY_SIZE(x)		(sizeof(x) / sizeof(*(x)))

void status_led_init();
void status_led_blink();
void status_led_toggle();
void status_led_on();
void status_led_off();
void host_interrupt_assert();
void host_interrupt_release();

#endif /* _UTIL_H_ */
