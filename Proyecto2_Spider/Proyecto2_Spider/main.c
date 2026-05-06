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

uint8_t posicion_S0 = 127;
uint8_t posicion_S1 = 127;
uint8_t posicion_S2 = 127;
uint8_t posicion_S3 = 127;
uint8_t posicion_S4 = 127;
uint8_t posicion_S5 = 127;

/*
servo 1 y 2 - patas delanteras		PD
servo 3 y 4 patas traseras			PT
servo 5 - brazo derecho adelante	BDA
servo 6 - brazo derecho atrás		BDR
servo 7 - brazo izquierdo adelante	BIA
servo 8 - brazo izquierdo atrás		BIR
*/  //se debe revisar

typedef enum {MODO_MANUAL, MODO_UART, MODO_EEPROM} modo_t;
volatile modo_t modo_actual = MODO_MANUAL;

/****************************************/
// Function prototypes
void setup(void);
void modo_manual(void);
void modo_uart(void);
void modo_eeprom(void);
void posicion_neutra(void);
void setServo(uint8_t servo, uint8_t valor);

/****************************************/
// Main Function

int main(void)
{
	setup();
	
	while (1)
	{
		switch (modo_actual)
		{
			case MODO_MANUAL:
			modo_manual();
			break;
			
			case MODO_UART:
			modo_uart();
			break;
			
			case MODO_EEPROM:
			modo_eeprom();
			break;
		}
		
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
	
	UART_sendString("Spider lista\r\n");
}

void modo_manual(void)
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

void modo_uart(void)
{
	if (bandera_UART)
	{
		bandera_UART = 0;
		
		switch (dato_UART)
		{
			case 'N':
			posicion_neutra();
			UART_sendString("Neutral\r\n");
			break;
			
			case 'M':
			modo_actual = MODO_MANUAL;
			UART_sendString("Modo manual\r\n");
			break;
			
			default:
			UART_sendString("Comando no válido\r\n");
			break;
		}
	}
}

void modo_eeprom(void)
{
	//lo veremos después
}

void posicion_neutra(void)
{
	setServo(0, 127);
	setServo(1, 127);
	setServo(2, 127);
	setServo(3, 127);
	setServo(4, 127);
	setServo(5, 127);
}

void setServo(uint8_t servo, uint8_t valor)
{
	uint8_t duty8 = 0;
	uint16_t duty16 = 0;
	
	switch (servo)
	{
		case 0:
		duty8 = 8 + ((uint32_t)valor * 27) / 255;
		dutyCycle_S0(duty8);
		posicion_S0 = valor;
		break;
		
		case 1:
		duty8 = 8 + ((uint32_t)valor * 27) / 255;
		dutyCycle_S1(duty8);
		posicion_S1 = valor;
		break;
		
		case 2:
		duty16 = 1000 + ((uint32_t)valor * 3500) / 255;
		dutyCycle_S2(duty16);
		posicion_S2 = valor;
		break;
		
		case 3:
		duty16 = 1000 + ((uint32_t)valor * 3500) / 255;
		dutyCycle_S3(duty16);
		posicion_S3 = valor;
		break;
		
		case 4:
		duty8 = 8 + ((uint32_t)valor * 27) / 255;
		dutyCycle_S4(duty8);
		posicion_S4 = valor;
		break;
		
		case 5:
		duty8 = 8 + ((uint32_t)valor * 27) / 255;
		dutyCycle_S5(duty8);
		posicion_S5 = valor;
		break;
	}
}

/****************************************/
// Interrupt routines