/*
 * UART.c
 *
 * Created: 5/1/2026 8:52:27 AM
 *  Author: andres barrientos
 */ 

#include "UART.h"

volatile char dato_UART = 0;
volatile uint8_t bandera_UART = 0;

void UART_init(void)
{
	//PD1 = TX (salida), PD0 = RX (entrada)
	DDRD |= (1 << DDD1);
	DDRD &= ~(1 << DDD0);
	
	UCSR0A = 0;		//modo normal
	
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);			//8 bits
	
	UBRR0 = 103;		//baudrate 9600 a 16MHz
}

void UART_sendChar(char caracter)
{
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = caracter;
}

void UART_sendString(char *texto)
{
	uint8_t i = 0;
	
	while (texto[i] != '\0')
	{
		UART_sendChar(texto[i]);
		i++;
	}
}

ISR(USART_RX_vect)
{
	dato_UART = UDR0;
	bandera_UART = 1;
}