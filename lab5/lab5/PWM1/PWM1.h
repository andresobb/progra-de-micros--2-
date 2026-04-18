/*
 * PWM1.h
 *
 * Created: 4/16/2026 10:09:19 AM
 *  Author: Andrés Barrientos
 */ 


#ifndef PWM1_H_
#define PWM1_H_

#include <avr/io.h>
#include <stdint.h>

void PWM1_init(void);
void update_DutyCycle0(uint16_t dutyCycle);

#endif /* PWM1_H_ */