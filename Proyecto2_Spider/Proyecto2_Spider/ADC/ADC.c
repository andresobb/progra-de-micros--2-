/*
 * ADC.c
 *
 * Created: 4/30/2026 4:44:28 PM
 *  Author: andres barrientos
 */ 

#include "ADC.h"

volatile uint16_t ADC_valores[6] = {0, 0, 0, 0, 0, 0};
volatile uint8_t canal_ADC = 0;

void ADC_init(void)
{
	ADMUX = (1 << REFS0);
	
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	
	DIDR0 = (1 << ADC0D) | (1 << ADC1D) | (1 << ADC2D) | (1 << ADC3D) | (1 << ADC4D) | (1 << ADC5D);
	
	ADCSRA |= (1 << ADSC);
}

ISR(ADC_vect)
{
	ADC_valores[canal_ADC] =  ADC;		//guarda la lectura del canal actual
	canal_ADC++;
	
	if (canal_ADC >= 6)
	{
		canal_ADC = 0;
	}
	
	ADMUX = (ADMUX & 0xF0) | (canal_ADC & 0x0F);
	
	ADCSRA |= (1 << ADSC);
	
}
