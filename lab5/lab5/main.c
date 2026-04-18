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
#include "PWM2/PWM2.h"

volatile uint16_t valor_ADC0 = 0;
volatile uint16_t valor_ADC1 = 0;
uint16_t copia_ADC0 = 0;
uint16_t copia_ADC1 = 0;
uint16_t dutyCycle0 = 0;
uint16_t dutyCycle1 = 0;

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
		copia_ADC0 = valor_ADC0;
		copia_ADC1 = valor_ADC1;
				
		dutyCycle0 = 1000 + ((uint32_t)copia_ADC0 * 3500) / 1023;
		dutyCycle1 = 8 + ((uint32_t)copia_ADC1 * 27) / 1023;
		update_DutyCycle0(dutyCycle0);	
		update_DutyCycle1(dutyCycle1);	
	}
	
}

/****************************************/
// NON-Interrupt subroutines
void setup(void)
{
	cli();
	
	DDRC &= ~((1 << DDC1) | (1 << DDC0));		//PC0 Y PC1 para ADC
	PORTC &= ~((1 << PORTC1) | (1 << PORTC0));
	DIDR0 |= ((1 << ADC1D) | (1 << ADC0D));		//desactiva entrada digital en el ADC
	
	ADC_init();
	PWM1_init();
	PWM2_init();
	
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
	uint8_t canal_ADC = ADMUX & 0x0F;
	
	if (canal_ADC == 0)
	{
		valor_ADC0 = ADC;
		ADMUX = (ADMUX & 0xF0) | 1;				// 0 -> 1
	}
	else if (canal_ADC == 1)
	{
		valor_ADC1 = ADC;
		ADMUX = (ADMUX & 0xF0) | 0;				// 1 -> 0
	}
	
	ADCSRA |= (1 << ADSC);
}