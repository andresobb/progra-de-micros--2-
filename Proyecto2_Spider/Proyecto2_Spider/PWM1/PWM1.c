/*
 * PWM1.c
 *
 * Created: 4/30/2026 5:32:17 PM
 *  Author: andre
 */ 

#include "PWM1.h"

void PWM1_init()
{
	//PB1 = 0CR1A, PB2 = OCR1B
	DDRB |= ((1 << DDB2) | (1 << DDB1));
	
	TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);		//fast modo 14, top ICR1, no inversor
	
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);			//prescaler de 8
	
	ICR1 = 39999;
	
	OCR1A = 3000;
	OCR1B = 3000;
}

void dutyCycle_S2(uint8_t dutyCycle)
{
	OCR1A =  dutyCycle;
}

void dutyCycle_S3(uint8_t dutyCycle)
{
	OCR1B =  dutyCycle;
}