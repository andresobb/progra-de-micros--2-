/*
* Proyecto1_Reloj.asm
*
* Creado: 2/24/2026 7:12:15 PM
* Autor : Andrés Barrientos
* Descripción: Reloj (Proyecto 1)
*/
/****************************************/
// Encabezado (Definición de Registros, Variables y Constantes)
.include "M328PDEF.inc"     // Include definitions specific to ATMega328P
.dseg
.org    SRAM_START

.cseg
.org 0x0000
	RJMP	SETUP
 /****************************************/
// Configuración de la pila
SETUP:
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

LDI		R16, 0xFF
OUT		DDRD, R16		;DISPLAYS de 7 segmentos
LDI		R16, 0x00
OUT		PORTD, R16

LDI		R16, 0xFF
OUT		DDRB, R16		//PB0-PB3 multiplex y PB4 BUZZER
LDI		R16, 0x00
OUT		PORTB, R16

LDI		R16, 0x38			//0011 1000	; PC0 - PC2 botones, PC3 - P5 LEDS DE ESTADOS
OUT		DDRC, R16
LDI		R16, 0x07			// pullups
OUT		PORTC, R16			


.equ T1VALUE = 0xFC30		//49,911 PARA 1s

CALL INIT_TMR0
CALL INIT_TMR1

    
/****************************************/
// Loop Infinito
MAIN_LOOP:
	RJMP    MAIN_LOOP

/****************************************/
// NON-Interrupt subroutines

INIT_TMR0:		;usamos prescaler de 64, esto sera para el multiplexado
	LDI		R16, (1 << CS01) | (1 << CS00)
	OUT		TCCR0B, R16

	LDI		R16, 177		//aprox 5ms
	OUT		TCNT0, R16

	RET

INIT_TMR1:
	LDI		R16, 0x00
	STS		TCCR1A, R16		//modo normal
			
	LDI		R16, (1 << CS11) | (1 << CS10)		//prescaler de 64
	STS		TCCR1B, R16

	LDI		R16, HIGH(T1VALUE)
	STS		TCNT1H, R16
	LDI		R16, LOW(T1VALUE)
	STS		TCNT1L, R16	
	
	RET

/****************************************/
// Interrupt routines

/****************************************/
