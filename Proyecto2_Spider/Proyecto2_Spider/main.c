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
- codo izq- PD6
- codo der - PD5
- brazo izq - PB1
- brazo der - PB2
- pata izq - PB3
- pata der - PD3 
*/

typedef enum {MODO_MANUAL, MODO_UART, MODO_EEPROM} modo_t;
volatile modo_t modo_actual = MODO_MANUAL;

char buffer_UART[10];
uint8_t index_UART = 0;

/****************************************/
// Function prototypes
void setup(void);
void modo_manual(void);
void modo_uart(void);
void modo_eeprom(void);
void posicion_neutra(void);
void setServo(uint8_t servo, uint8_t valor);
void procesar_comando_uart(void);
void LED_init(void);
void update_LED(void);

/****************************************/
// Main Function

int main(void)
{
	setup();
	
	while (1)
	{
		if (bandera_UART)
		{
			if (dato_UART == 'U')
			{
				bandera_UART = 0;
				index_UART = 0;
				modo_actual = MODO_UART;
				update_LED();
				UART_sendString("\r\nModo UART\r\n");
			}
			
			else if (dato_UART == 'M')
			{
				bandera_UART = 0;
				index_UART = 0;
				modo_actual = MODO_MANUAL;
				update_LED();
				UART_sendString("\r\nModo manual\r\n");	
			}
			
			else if (modo_actual != MODO_UART)
			{
				bandera_UART = 0;
			}
		}	
	
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
	LED_init();
	
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
		
		if (dato_UART == '\r' || dato_UART == '\n')
		{
			if (index_UART > 0)
			{
				buffer_UART[index_UART] = '\0';
				procesar_comando_uart();
				index_UART = 0;
			}
		}
		
		else
		{
			if (index_UART < 9)
			{
				buffer_UART[index_UART] = dato_UART;
				index_UART++;
			}
			
			else 
			{
				index_UART = 0;
				UART_sendString("Buffer lleno\r\n");
			}
		}
	}
}

void procesar_comando_uart()
{
	uint8_t servo = 0;
	uint16_t valor = 0;
	uint8_t i = 0;
	
	if (buffer_UART[0] == 'N')	//llamamos posicion neutra	
	{
		posicion_neutra();
		UART_sendString("Servos en posicion neutra.\r\n");
		return;
	}
	if (buffer_UART[0] == 'M')		//por si se quiere modo manual
	{
		modo_actual = MODO_MANUAL;
		UART_sendString("Modo manual activado.\r\n");
		return;
	}
	if (buffer_UART[0] != 'S')		// usaremos un formato S0:255, por ejemplo. aca verificamos que se este ingresando el formato correcto
	{
		UART_sendString("Comando invalido.\r\n");
		return;
	}
	if (buffer_UART[1] < '0' || buffer_UART[1] > '5')
	{
		UART_sendString("Servo invalido.\r\n");
		return;
	}
	if (buffer_UART[2] != ':')
	{
		UART_sendString("Formato invalido.\r\n");
		return;
	}
	
	servo = buffer_UART[1] - '0';		//asignamos el valor del servo
	
	i = 3;			//empezamos en 3 porque son los de la posicion (3, 4 y 5)
	
	while (buffer_UART[i] >= '0' && buffer_UART[i] <= '9')
	{
		valor = (valor * 10) + (buffer_UART[i] - '0');
		i++;
	}
	
	if (valor > 255)
	{
		valor = 255;
	}
	
	setServo(servo, (uint8_t)valor);
	UART_sendString("Ejecucion completada\r\n");
	
}

void modo_eeprom(void)
{
	//lo veremos después
}

void posicion_neutra(void)
{
	setServo(0, 255);
	setServo(1, 0);
	setServo(2, 160);
	setServo(3, 120);
	setServo(4, 0);
	setServo(5, 160);
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

void LED_init(void)
{
	DDRD |= ((1 << DDD7) | (1 << DDD4) | (1 << DDD2));
	
	//rgb es anodo comun, por lo que 1 es apagado
	PORTD |= ((1 << PORTD7) | (1 << PORTD4) | (1 << PORTD2));
	
	update_LED();		
}

void update_LED(void)
{
	//PD2 - ROJO, PD4 - VERDE, PD7 - AZUL
	
	PORTD |= ((1 << PORTD7) | (1 << PORTD4) | (1 << PORTD2));
	
	switch (modo_actual)
	{
		case MODO_MANUAL:
		PORTD &= ~((1 << PORTD7) | (1 << PORTD2));		//morado para manual
		break;
		
		case MODO_UART:
		PORTD &= ~((1 << PORTD7) | (1 << PORTD4));		//cyan para UART pq es el mas cool
		break;
		
		case MODO_EEPROM:
		PORTD &= ~((1 << PORTD4) | (1 << PORTD2));		//morado para manual
		break;
	}
}

/****************************************/
// Interrupt routines