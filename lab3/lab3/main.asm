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

 /****************************************/
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

LDI		R16, 0x00
OUT		DDRB, R16
LDI		R16, 0x03			;activamos pullups en los primeros dos bits de portB (botones)
OUT		PORTB, R16

//primero ponemos que usaremos el puerto B
LDI		R16, (1 << PCIE0)
STS		PCICR, R16

//usaremos PB0 y PB1
LDI		R16, (1 << PCINT1) | (1 << PCINT0)
STS		PCMSK0, R16


/******************* VARIABLES *********************/
TEMP = R17		; para la  isr


   
/****************************************/
// Loop Infinito
MAIN:
    RJMP    MAIN

/****************************************/
// NON-Interrupt subroutines
PCINT0_ISR:
	PUSH	R16
	IN		R16, SREG
	PUSH	R16
	PUSH	TEMP


	POP		TEMP
	POP		R16
	OUT		R16, SREG
	POP		R16

	RETI
	

/****************************************/
// Interrupt routines

/****************************************/





; necesitamos activar PCMSK0 y PCIE0 porque es puerto B, PCINT0 Y PCINT1 por PB0 y PB1