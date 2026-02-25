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


    
/****************************************/
// Loop Infinito
MAIN_LOOP:
    RJMP    MAIN_LOOP

/****************************************/
// NON-Interrupt subroutines

/****************************************/
// Interrupt routines

/****************************************/
