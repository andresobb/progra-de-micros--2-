/*
 * PWM3.h
 *
 * Created: 4/17/2026 7:00:55 PM
 *  Author: andre
 */ 


#ifndef PWM3_H_
#define PWM3_H_

#include <avr/io.h>

void PWM3_init(void);
void update_DutyCycle2(uint8_t dutyCycle);

#endif /* PWM3_H_ */