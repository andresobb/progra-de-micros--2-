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

segdig: .dB 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x18, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E

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

LDI		R16, 0x0F  			//contador cada 10ms en puerto C
OUT		DDRC, R16
LDI		R16, 0x00
OUT		PORTC, R16

LDI		R16, 0xFF
OUT		DDRD, R16
LDI		R16, 0x00
OUT		PORTD, R16

LDI		R16, 0x04		//ponemos aca el LED de alarma. 4 - 0100 (bit 3 como salida)
OUT		DDRB, R16
LDI		R16, 0x03
OUT		PORTB, R16

.def CONTADOR = R23
.def OVERFLOW = R24
.def CONTADOR7SEG = R25
.def BOTONES = R26
.def LED = R27

CLR	CONTADOR
CLR	OVERFLOW
CLR CONTADOR7SEG
CLR	R0
CLR	LED

IN	BOTONES, PINB

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
		CPI		OVERFLOW, 100

		BRNE	FINMAIN
		CLR		OVERFLOW

		INC		CONTADOR
		ANDI	CONTADOR, 0x0F

		CP		CONTADOR, CONTADOR7SEG
		BRNE	NO_MATCH

		OUT		PORTC, CONTADOR
		CLR		CONTADOR

		LDI		R16, 0x04		//cargamos 0100 para solo tocar el bit 3
		EOR		LED, R16

		; re construimos el puerto B con los pullups en los botones
		LDI		R16, 0x03
		OR		R16, LED
		OUT		PORTB, R16
		
		RJMP    FINMAIN

NO_MATCH:
		OUT		PORTC, CONTADOR

FINMAIN:
	CALL	BOTONES_fnc
	RJMP	MAIN_LOOP

/****************************************/
// NON-Interrupt subroutines

INIT_TMR0:		;usamos prescaler de 64
	LDI		R16, (1 << CS01) | (1 << CS00)
	OUT		TCCR0B, R16

	LDI		R16, 100
	OUT		TCNT0, R16

	RET

BOTONES_fnc:
	IN		R16, PINB
	CP		BOTONES, R16
	BREQ	FIN_BOTONES

	CALL	DELAY

	IN		R16, PINB
	CP		BOTONES, R16
	BREQ	FIN_BOTONES

	MOV		BOTONES, R16

	SBRS	BOTONES, 0
	CALL	INC_CONTADOR7SEG

	SBRS	BOTONES, 1
	CALL	DEC_CONTADOR7SEG

	LDI		ZH, HIGH(segdig << 1)
	LDI		ZL, LOW(segdig << 1)

	ADD		ZL, CONTADOR7SEG
	ADC		ZH, R0
	LPM		R19, Z
	OUT		PORTD, R19
	
	RET

FIN_BOTONES:
	RET

INC_CONTADOR7SEG:
	INC		CONTADOR7SEG
	CPI		CONTADOR7SEG, 16
	BRNE	INC_LISTO
	CLR		CONTADOR7SEG

INC_LISTO:
	RET

DEC_CONTADOR7SEG:
	DEC		CONTADOR7SEG
	CPI		CONTADOR7SEG, 255
	BRNE	DEC_LISTO
	LDI		CONTADOR7SEG, 15

DEC_LISTO:
	RET

DELAY:
	LDI		R20, 0xFF
DELAY_1:
	DEC		R20
	BRNE	DELAY_1
	RET			

/****************************************/
// Interrupt routines

/****************************************/