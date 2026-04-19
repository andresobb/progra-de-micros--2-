/*
 * PWM3.c
 *
 * Created: 4/17/2026 7:00:44 PM
 *  Author: Andres Barrientos
 */ 

#include "PWM3.h"

volatile uint8_t contador = 0;
volatile uint8_t limite_PWM3 = 0;

void PWM3_init(void)
{
	DDRD |= (1 <<DDD2);
	PORTD &= ~(1 << PORTD2);
	
	TCCR0A = 0;
	TCCR0B = 0;
	
	TCCR0A |= (1 << WGM01);		// usamos modo CTC porque aca es mas facil
	TCCR0B |= (1 << CS01) | (1 << CS00);		//prescaler de 64
	
	OCR0A = 24;
	
	TIMSK0 |= (1 << OCIE0A);		//interrupcion por compare match A	
}

void update_DutyCycle2(uint8_t dutyCycle)
{
	if (dutyCycle > 100)
	{
		dutyCycle = 100;
	}	
	limite_PWM3 = dutyCycle;
}

ISR(TIMER0_COMPA_vect)
{
	if (contador < limite_PWM3)
	{
		PORTD |= (1 << PORTD2);
	}
	else
	{
		PORTD &= ~(1 << PORTD2);
	}
	
	contador++;
	
	if (contador >= 100)
	{
		contador = 0;	
	}
}