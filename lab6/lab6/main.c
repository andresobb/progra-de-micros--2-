/*
 * lab6.c
 *
 * Created: 4/23/2026 12:25:32 PM
 * Author: Andrés Barrientos
 * Description: 
 */
/****************************************/
// Encabezado (Libraries)
#define F_CPU 16000000
#include <avr/io.h>	
#include <stdint.h>

/****************************************/
// Function prototypes
void setup(void);
void UART_init(void);
void UART_sendChar(char caracter);
char UART_readChar(void);
void mostrarASCII(uint8_t dato);

/****************************************/
// Main Function
int main(void)
{
	setup();
	
	// parte 1: enviar un caracter una vez al encender
	UART_sendChar('A');
	
	while (1)
	{
		//paete 2: recibir un caracter y mostrarlo en los leds
		char recibido = UART_readChar();
		mostrarASCII((uint8_t)recibido);		
	}
}

/****************************************/
// NON-Interrupt subroutines
void setup(void)
{
	DDRB |= 0x3F;		// PB0-PB5 como salida
	DDRD |= ((1 << DDD6) | (1 << DDD7));		// PD6 Y PD7
	
	PORTB &= ~0x3F;
	PORTD &= ~((1 << PORTD6) | (1 << PORTD7));
	
	UART_init();
}

void UART_init(void)
{
	
	// 9600 baudios para 16MHz
	UBRR0H = 0;
	UBRR0L = 103;
	
	UCSR0A = 0;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);		//habilitamos la transmision y recepcion
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);		//datos de 8 bits
}

void UART_sendChar(char caracter)
{
	while (!(UCSR0A & (1 << UDRE0)));			//espera hasta que el registro de transmision este listo
	UDR0 = caracter;
}

char UART_readChar(void)
{
	while (!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

void mostrarASCII(uint8_t dato)
{
	// bits 0-5 a PB0-PB5
	PORTB = (PORTB & 0xC0) | (dato & 0x3F);
	
	// bits 6-7 a PD6-PD7
	PORTD = (PORTD & 0x3F) | (dato & 0xC0);
}
/****************************************/
// Interrupt routines