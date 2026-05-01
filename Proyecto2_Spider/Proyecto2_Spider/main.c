/*
 * Proyecto2_Spider.c
 *
 * Created:4/30/2026 4:26:11 PM 
 * Author: Andrés Barrientos
 * Description: Araña con servomotores
 */
/****************************************/
// Encabezado (Libraries)
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "ADC/ADC.h"
#include "PWM0/PWM0.h"
#include "PWM1/PWM1.h"
#include "PWM2/PWM2.h"
#include "UART/UART.h"

/****************************************/
// variables
uint16_t copia_ADC0;
uint16_t copia_ADC1;
uint16_t copia_ADC2;
uint16_t copia_ADC3;
uint16_t copia_ADC4;
uint16_t copia_ADC5;

uint8_t dutyCycle0;
uint8_t dutyCycle1;
uint16_t dutyCycle2;
uint16_t dutyCycle3;
uint8_t dutyCycle4;
uint8_t dutyCycle5;

/****************************************/
// Function prototypes
void setup(void);

/****************************************/
// Main Function

int main(void)
{
	setup();
	
	while (1)
	{
		cli();		
		copia_ADC0 = ADC_valores[0];
		copia_ADC1 = ADC_valores[1];
		copia_ADC2 = ADC_valores[2];
		copia_ADC3 = ADC_valores[3];
		copia_ADC4 = ADC_valores[4];
		copia_ADC5 = ADC_valores[5];
		
		sei();
				
		dutyCycle0 = 8 + ((uint32_t)copia_ADC0 * 27) / 1023;	//OCR0A
		dutyCycle1 = 8 + ((uint32_t)copia_ADC1 * 27) / 1023;	//OCR0B
		
		dutyCycle2 = 1000 + ((uint32_t)copia_ADC2 * 3500) / 1023;	//OCR1A
		dutyCycle3 = 1000 + ((uint32_t)copia_ADC3 * 3500) / 1023;	//OCR1B
		
		dutyCycle4 = 8 + ((uint32_t)copia_ADC4 * 27) / 1023;	//OCR2A
		dutyCycle5 = 8 + ((uint32_t)copia_ADC5 * 27) / 1023;	//OCR2B
		
		dutyCycle_S0(dutyCycle0);
		dutyCycle_S1(dutyCycle1);
		dutyCycle_S2(dutyCycle2);
		dutyCycle_S3(dutyCycle3);
		dutyCycle_S4(dutyCycle4);
		dutyCycle_S5(dutyCycle5);
		
	}
}

/****************************************/
// NON-Interrupt subroutines
void setup(void)
{
	cli();
	
	ADC_init();
	PWM0_init();
	PWM1_init();
	PWM2_init();
	UART_init();
	
	sei();
}

/****************************************/
// Interrupt routines