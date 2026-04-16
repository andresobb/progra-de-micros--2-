/*
 * PWM1.c
 *
 * Created: 4/16/2026 10:09:35 AM
 *  Author: Andrés Barrientos
 */ 

#include "PWM1.h"

void PWM1_init(void)
{
	DDRB |= (1 << DDB1);
	
	TCCR1A = 0;
	TCCR1B = 0;
	
	TCCR1A |= (1 << COM1A1) | (1 << WGM11);		//FAST modo no invertido + modo 14
	TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS11);	//completa modo 14 + prescaler de 8
	
	ICR1 = 39999;			//periodo de 20ms 
	OCR1A = 2750;			// pulso inicial 1.5 ms aproximado
}

void update_DutyCycle1(uint16_t dutyCycle)
{
	OCR1A = dutyCycle;
}