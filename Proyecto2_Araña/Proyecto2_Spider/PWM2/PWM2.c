/*
 * PWM2.c
 *
 * Created: 4/30/2026 5:39:51 PM
 *  Author: andres barrientos
 */ 

#include "PWM2.h"

void PWM2_init(void)
{
	//PB3 = OCR2A, PD3 = OCR2B
	DDRB |= (1 << DDB3);
	DDRD |= (1 << DDD3);
			
	TCCR2A = (1 << COM2A1) | (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);		//fast pwm, no invertido
	
	TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);		//prescaler 1024
	
	OCR2A = 21;
	OCR2B = 21;
}

void dutyCycle_S4(uint8_t dutyCycle)
{
	OCR2A =  dutyCycle;
}

void dutyCycle_S5(uint8_t dutyCycle)
{
	OCR2B =  dutyCycle;
}