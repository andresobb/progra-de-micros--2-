/*
 * PWM2.h
 *
 * Created: 4/30/2026 5:39:24 PM
 *  Author: andres barrientos
 */ 


#ifndef PWM0_H_
#define PWM0_H_

#include <avr/io.h>
#include <stdint.h>

void PWM0_init(void);
void dutyCycle_S0(uint8_t dutyCycle);
void dutyCycle_S1(uint8_t dutyCycle);

#endif /* PWM2_H_ */