/*
 * lab5.c
 *
 * Created: 4/12/2026 11:16:18 PM
 * Author: Andrés Barrientos
 * Description: 
 */
/****************************************/
// Encabezado (Libraries)
#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "PWM1/PWM1.h"

volatile uint16_t valor_ADC = 0;
uint16_t copia_ADC = 0;
uint16_t dutyCycle = 0;

/****************************************/
// Function prototypes
void setup(void);
void ADC_init(void);

/****************************************/
// Main Function
int main(void)
{
	setup();
	
	while (1)
	{
		cli();
		copia_ADC = valor_ADC;
		sei();
		
		dutyCycle = 1000 + ((uint32_t)copia_ADC * 4000) / 1023;
		update_DutyCycle1(dutyCycle);		
	}
	
}

/****************************************/
// NON-Interrupt subroutines
void setup(void)
{
	cli();
	
	DDRC &= ~(1 << DDC0);
	PORTC &= ~(1 << PORTC0);
	DIDR0 |= (1 << ADC0D);		//desactiva entrada digital en el ADC
	
	ADC_init();
	PWM1_init();
	
	sei();	
}

void ADC_init(void)
{
	ADMUX = 0;
	ADMUX |= (1 << REFS0);		//referencia AVcc, MUX 0000 para ADC0
	
	ADCSRA = 0;
	ADCSRA |= ((1 << ADEN) | (1 << ADIE) | (1 << ADPS2 ) | (1 << ADPS1) | (1 << ADPS0));	//encendemos ADC, prescaler 128 (16MHz/128 = 125kHz). ADIE interrupt enable
	
	ADCSRA |= (1 << ADSC);		//iniciamos la conversion
}

/****************************************/
// Interrupt routines
ISR(ADC_vect)						//evitamos polling
{
	valor_ADC = ADC;
	ADCSRA |= (1 << ADSC);		//guardamos dato y volvemos a leer
}