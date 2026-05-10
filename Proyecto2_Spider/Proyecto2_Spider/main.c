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
#include "EEPROM/EEPROM.h"

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
void saludo_izq(void);
void saludo_der(void);
void reverencia(void);
void setServo(uint8_t servo, uint8_t valor);
void procesar_comando_uart(void);
void LED_init(void);
void update_LED(void);
void guardar_pose(uint8_t pose);
void leer_pose(uint8_t pose);
void menu_eeprom();

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
			
			else if (dato_UART == 'E')
			{
				bandera_UART = 0;
				index_UART = 0;
				modo_actual = MODO_EEPROM;
				update_LED();
				menu_eeprom();
				
			}
			
			else if (modo_actual != MODO_UART && modo_actual != MODO_EEPROM)
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
	
	posicion_S0 = copia_ADC0 / 4;
	posicion_S1 = copia_ADC1 / 4;
	posicion_S2 = copia_ADC2 / 4;
	posicion_S3 = copia_ADC3 / 4;
	posicion_S4 = copia_ADC4 / 4;
	posicion_S5 = copia_ADC5 / 4;
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
	
	if (buffer_UART[0] == 'I')	//saluda con la mano izquierda
	{
		saludo_izq();
		UART_sendString("Saludo con la mano izquierda.\r\n");
		return;
	}
	
	if (buffer_UART[0] == 'D')	//saludo con la mano derecha
	{
		saludo_der();
		UART_sendString("Saludo con la mano derecha.\r\n");
		return;
	}
	
	if (buffer_UART[0] == 'R')	//hace reverencia
	{
		reverencia();
		UART_sendString("Your highness.\r\n");
		return;
	}
	
	if (buffer_UART[0] == 'M')		//por si se quiere modo manual
	{
		modo_actual = MODO_MANUAL;
		update_LED();
		UART_sendString("Modo manual activado.\r\n");
		return;
	}
	
	if (buffer_UART[0] == 'E')		//por si se quiere modo manual
	{
		modo_actual = MODO_EEPROM;
		update_LED();
		UART_sendString("Modo EEPROM activado.\r\n");
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
	if (bandera_UART)
	{
		bandera_UART = 0;
		if (dato_UART == '\r' || dato_UART == '\n')
		{
			if (index_UART > 0)
			{
				buffer_UART[index_UART] = '\0';
				
				if (buffer_UART[0] == 'M')
				{
					modo_actual = MODO_MANUAL;
					update_LED();
					UART_sendString("Modo manual\r\n");
					index_UART = 0;
					return;
				}
				
				if (buffer_UART[0] == 'U')
				{
					modo_actual = MODO_UART;
					update_LED();
					UART_sendString("Modo UART\r\n");
					index_UART = 0;
					return;
				}
				
				if (buffer_UART[0] == 'G')
				{
					if (buffer_UART[1] >= '0' && buffer_UART[1] <= '3')
					{
						guardar_pose(buffer_UART[1] - '0');
					}
					else
					{
						UART_sendString("Pose no valida\r\n");
					}
				}
				else if (buffer_UART[0] == 'L')
				{
					if (buffer_UART[1] >= '0' && buffer_UART[1] <= '3')
					{
						leer_pose(buffer_UART[1] - '0');
					}
					else
					{
						UART_sendString("Pose no valida\r\n");
					}
				}
				else
				{
					UART_sendString("Comando no valido\r\n");
					menu_eeprom();
				}
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
				UART_sendString("Buffer lleno\r\n"); // cuando pasara esto? no entiendo
			}
		}
	}
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

void saludo_izq()
{
	setServo(0, 0);
	setServo(1, 0);
	setServo(2, 100);
	setServo(3, 100);
	setServo(4, 90);
	setServo(5, 160);
}

void saludo_der()
{
	setServo(0, 255);
	setServo(1, 255);
	setServo(2, 200);
	setServo(3, 120);
	setServo(4, 20);
	setServo(5, 100);
}

void reverencia()
{
	setServo(0, 180);
	setServo(1, 100);
	setServo(2, 160);
	setServo(3, 120);
	setServo(4, 100);
	setServo(5, 40);
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
		PORTD &= ~(1 << PORTD2);		//eeprom en rojo
		break;
	}
}

void guardar_pose(uint8_t pose)
{
	uint16_t direccion = pose * 6;  //por que 6?
	
	EEPROM_write(direccion + 0, posicion_S0);
	EEPROM_write(direccion + 1, posicion_S1);
	EEPROM_write(direccion + 2, posicion_S2);
	EEPROM_write(direccion + 3, posicion_S3);
	EEPROM_write(direccion + 4, posicion_S4);
	EEPROM_write(direccion + 5, posicion_S5);
	
	UART_sendString("\r\nPosicion guardada\r\n");
}

void leer_pose(uint8_t pose)
{
	uint16_t direccion = pose * 6;
	
	uint8_t s0 = EEPROM_read(direccion + 0);
	uint8_t s1 = EEPROM_read(direccion + 1);
	uint8_t s2 = EEPROM_read(direccion + 2);
	uint8_t s3 = EEPROM_read(direccion + 3);
	uint8_t s4 = EEPROM_read(direccion + 4);
	uint8_t s5 = EEPROM_read(direccion + 5);
	
	setServo(0, s0);
	setServo(1, s1);
	setServo(2, s2);
	setServo(3, s3);
	setServo(4, s4);
	setServo(5, s5);
	
	UART_sendString("\r\nPose leida\r\n");	
}

void menu_eeprom()
{
	UART_sendString("\r\nModo EEPROM\r\n");
	UART_sendString("G0 - Guardar posicion 0\r\n");
	UART_sendString("G1 - Guardar posicion 1\r\n");
	UART_sendString("G2 - Guardar posicion 2\r\n");
	UART_sendString("G3 - Guardar posicion 3\r\n");
	UART_sendString("L0 - Leer posicion 0\r\n");
	UART_sendString("L1 - Leer posicion 1\r\n");
	UART_sendString("L2 - Leer posicion 2\r\n");
	UART_sendString("L3 - Leer posicion 3\r\n");
	UART_sendString("M  - Volver a modo manual\r\n");
	UART_sendString("U  - Volver a modo UART\r\n");
}

/****************************************/
// Interrupt routines