#ifndef _UTIL_H_
#define _UTIL_H_

#define true	1
#define false	0

#define ARRAY_SIZE(x)		(sizeof(x) / sizeof(*(x)))

void status_led_init();
void status_led_blink();
void status_led_toggle();

#endif /* _UTIL_H_ */
