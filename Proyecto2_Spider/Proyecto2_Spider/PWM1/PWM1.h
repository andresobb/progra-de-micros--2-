/*
 * PWM1.h
 *
 * Created: 4/30/2026 5:32:26 PM
 *  Author: andre
 */ 


#ifndef PWM1_H_
#define PWM1_H_

#include <avr/io.h>
#include <stdint.h>

void PWM1_init(void);
void dutyCycle_S2(uint16_t dutyCycle);
void dutyCycle_S3(uint16_t dutyCycle);

#endif /* PWM1_H_ */