/*
 * PWM2.c
 *
 * Created: 4/17/2026 12:01:46 PM
 *  Author: Andres Barrientos
 */ 

#include "PWM2.h"

void PWM2_init(void)
{
	DDRD |= (1 << DDD3);			//OC2B
	
	TCCR2A = 0;
	TCCR2B = 0;
	
	TCCR2A |= (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);		//FAST PWM no invertido
	TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);	//prescaler 1024
	
	OCR2B = 21;
}

void update_DutyCycle1(uint16_t dutyCycle)
{
	OCR2B = dutyCycle;
}