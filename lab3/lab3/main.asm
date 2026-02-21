/*
* lab3.asm
*
* Creado: 2/17/2026 7:28:17 PM
* Autor : Andrés Barrientos
* Descripción: Prelab - 
*/
/****************************************/
// Encabezado (Definición de Registros, Variables y Constantes)
.include "M328PDEF.inc"     // Include definitions specific to ATMega328P
.dseg
.org    SRAM_START

.cseg
.org 0x0000
		RJMP SETUP
.org PCI0addr		;0x0006
	RJMP PCINT0_ISR
.org OVF0addr		;0x0020 overflow del timer 0
	RJMP  TIMER0_ISR

 /****************************************/
SETUP:

segdig: .dB 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x18

// Configuración de la pila
LDI     R16, LOW(RAMEND)
OUT     SPL, R16
LDI     R16, HIGH(RAMEND)
OUT     SPH, R16
/****************************************/
// Configuracion MCU 

LDI		R16, (1 << CLKPCE)
STS		CLKPR, R16
LDI		R16, 0x04		
STS		CLKPR, R16

LDI		R16, 0x0F
OUT		DDRC, R16
LDI		R16, 0x00
OUT		PORTC, R16			;seteamos el puerto C para el contador

LDI		R16, 0x2C			;puerto B en 1100
OUT		DDRB, R16
LDI		R16, 0x03			;activamos pullups en los primeros dos bits de portB (botones)
OUT		PORTB, R16

LDI		R16, 0xFF
OUT		DDRD, R16
LDI		R16, 0x00
OUT		PORTD, R16			;puerto D para el display

//primero ponemos que usaremos el puerto B
LDI		R16, (1 << PCIE0)
STS		PCICR, R16

//usaremos PB0 y PB1
LDI		R16, (1 << PCINT1) | (1 << PCINT0)
STS		PCMSK0, R16



SEI

CALL	INIT_TMR0
//ints por overflow
LDI		R16, (1 << TOIE0)	
STS		TIMSK0, R16
/******************* VARIABLES *********************/
.def BOTONES = R18		
.def CONTADOR4BITS = R19
.def OVERFLOW = R20
.def SEG_U = R22
.def SEG_D = R25
.def DIGITO = R23
.def SALIDA7 = R24
.def MULTIPLEX = R26

CLR	BOTONES
CLR	CONTADOR4BITS
CLR	OVERFLOW
CLR	R0
CLR SEG_U
CLR	SEG_D
CLR	DIGITO
CLR SALIDA7
CLR	MULTIPLEX
   
/****************************************/
// Loop Infinito
MAIN:
    RJMP    MAIN

/****************************************/
// NON-Interrupt subroutines

INIT_TMR0:	
	LDI		R16, (1 << CS01) | (1 << CS00)	;reciclamos del lab anterior
	OUT		TCCR0B, R16

	LDI		R16, 100
	OUT		TCNT0, R16

	RET

/****************************************/
// Interrupt routines

PCINT0_ISR:
	PUSH	R16
	IN		R16, SREG
	PUSH	R16
	PUSH	BOTONES

	IN		BOTONES, PINB

	SBRS	BOTONES, 0
	INC		CONTADOR4BITS

	SBRS	BOTONES, 1
	DEC		CONTADOR4BITS

	ANDI	CONTADOR4BITS, 0x0F
	OUT		PORTC, CONTADOR4BITS

	POP		BOTONES
	POP		R16
	OUT		SREG, R16
	POP		R16

	RETI

TIMER0_ISR:
	PUSH	R16
	IN		R16, SREG
	PUSH	R16
	PUSH	R19
	PUSH	R30
	PUSH	R31

	LDI		R16, 100
	OUT		TCNT0, R16

	INC		OVERFLOW
	CPI		OVERFLOW, 100
	BRNE	NO_CONTAR
	CLR		OVERFLOW
	
	SBI		PINB, 5

	INC		SEG_U
	CPI		SEG_U, 10
	BRNE	NO_CARRY
	CLR		SEG_U
	INC		SEG_D
	CPI		SEG_D, 6
	BRNE    NO_CARRY

	CLR		SEG_U
	CLR		SEG_D

	NO_CONTAR:
	NO_CARRY:

	;hacemos el multiplexado
	LDI		R16, 0x01
	EOR		MULTIPLEX, R16

	CPI		MULTIPLEX, 0
	BREQ	DIP_UNIDADES

	DIP_DECENAS: 
	SBI		PORTB, 3
	CBI		PORTB, 2

	MOV		DIGITO, SEG_D
	RJMP	MOSTRAR


	DIP_UNIDADES:
	CBI		PORTB, 3
	SBI		PORTB, 2

	MOV		DIGITO, SEG_U
	RJMP	MOSTRAR

	MOSTRAR:
	LDI		ZH, HIGH(segdig << 1)
	LDI		ZL, LOW(segdig << 1)

	ADD		ZL, DIGITO
	ADC		ZH, R0
	LPM		R19, Z
	OUT		PORTD, R19

	POP		R31
	POP		R30
	POP		R19
	POP		R16
	OUT		SREG, R16
	POP		R16

	RETI

/****************************************/





; necesitamos activar PCMSK0 y PCIE0 porque es puerto B, PCINT0 Y PCINT1 por PB0 y PB1