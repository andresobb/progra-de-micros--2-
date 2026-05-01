/*
 * UART.h
 *
 * Created: 5/1/2026 8:52:12 AM
 *  Author: andres barrientos
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

void UART_init(void);
void UART_sendChar(char caracter);
void UART_sendString(char *texto);	

extern volatile char dato_UART;
extern volatile uint8_t bandera_UART;

#endif /* UART_H_ */