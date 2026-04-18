/*
 * PWM2.h
 *
 * Created: 4/17/2026 12:02:01 PM
 *  Author: Andres Barrientos
 */ 


#ifndef PWM2_H_
#define PWM2_H_

#include <avr/io.h>
#include <stdint.h>

void PWM2_init(void);
void update_DutyCycle1(uint16_t dutyCycle);


#endif /* PWM2_H_ */