/*
* lab2.asm
*
* Creado: 2/13/2026 9:55:48 AM
* Autor : Andrés Barrientos
* Descripción: Prelab: Contador de 4bits automático 
*/
/****************************************/
// Encabezado (Definición de Registros, Variables y Constantes)
.include "M328PDEF.inc"     // Include definitions specific to ATMega328P
.dseg
.org    SRAM_START

.cseg
.org 0x0000
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

LDI		R16, 0x0F			//contador cada 100ms en puerto C
OUT		DDRC, R16
LDI		R16, 0x00
OUT		PORTC, R16

.def CONTADOR = R23
.def OVERFLOW = R24

CLR	CONTADOR
CLR	OVERFLOW

CALL INIT_TMR0

    
/****************************************/
// Loop Infinito
MAIN_LOOP:

	ESPERA_OVF:
		IN		R16, TIFR0
		SBRS	R16, TOV0		//bit 0 de TIFR0
		RJMP	ESPERA_OVF

		SBI		TIFR0, TOV0
		LDI		R16, 100
		OUT		TCNT0, R16

		INC		OVERFLOW
		CPI		OVERFLOW, 10

		BRNE	ESPERA_OVF
		CLR		OVERFLOW

		INC		CONTADOR
		ANDI	CONTADOR, 0x0F
		OUT		PORTC, CONTADOR
		
		RJMP    MAIN_LOOP

/****************************************/
// NON-Interrupt subroutines

INIT_TMR0:		;usamos prescaler de 64
	LDI		R16, (1 << CS01) | (1 << CS00)
	OUT		TCCR0B, R16

	LDI		R16, 100
	OUT		TCNT0, R16

	RET

/****************************************/
// Interrupt routines

/****************************************/