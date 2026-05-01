/*
 * PWM2.h
 *
 * Created: 4/30/2026 5:39:24 PM
 *  Author: andres barrientos
 */ 


#ifndef PWM2_H_
#define PWM2_H_

#include <avr/io.h>
#include <stdint.h>

void PWM2_init(void);
void dutyCycle_S4(uint8_t dutyCycle);
void dutyCycle_S5(uint8_t dutyCycle);

#endif /* PWM2_H_ */