#ifndef _TMR_H_
#define _TMR_H_

/*
 * This value was chosen to get 1Hz (Ftimer) interrupt out of the internal clock
 * oscillator of 8MHz (Fsys). Formula was deduced knowing that:
 *
 *  Ftimer = Fsys / prescaler / timer_max_value
 *
 *  Hence, the timer_max_value = Fsys / prescaler / Ftimer
 *
 *  Since the timer counter direction is up, we have to set the counter to:
 *
 *  tmr1_value = timer_max_value - Fsys / prescaler / Ftimer
 *
 *  So, for a prescaler of 256 and using the 16bit timer1, we get:
 *  tmr1_value = 2^16 - 8000000 / 256 / 1 = 65536 - 8000000 / 256 = 34286
 */
#define TMR1_VALUE			34286

/*
 * Due to the fact that internal oscillator is not very precise, the clock will
 * not be exactly 8MHz, hence the time will drift. This correction coefficient
 * is used for calibration and it's deduced through testing.
 */
#define TMR1_CORRECTION			(-775)

#define TMR1_RESET_VALUE		(TMR1_VALUE + TMR1_CORRECTION)

#define TMR1_CLOCK_SELECT_POS		CS10
#define TMR1_CLOCK_SELECT_MASK		(CS10 | CS11 | CS12)

void tmr1_init();
void tmr1_start(uint16_t tmr_value);
void tmr1_stop();

#endif
