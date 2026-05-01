/*
 * PWM0.c
 *
 * Created: 4/30/2026 5:14:41 PM
 *  Author: andres barrientos
 */ 

#include "PWM0.h"

void PWM0_init(void)
{
	// PD6 = 0CR0A, PD5 = 0CR0B
	DDRD |= (1 << DDD6) | (1 << DDD5);
	
	TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);		//fast pwm, no invertido
	
	TCCR0B = (1 << CS02) | (1 << CS00);		//prescaler 1024
	
	OCR0A = 21;
	OCR0B = 21;	
}

void dutyCycle_S0(uint8_t dutyCycle)
{
	OCR0A =  dutyCycle;
}

void dutyCycle_S1(uint8_t dutyCycle)
{
	OCR0B =  dutyCycle;
}