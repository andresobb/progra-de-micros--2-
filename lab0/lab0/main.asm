;
; lab0.asm
;
; Created: 1/23/2026 5:06:47 PM
; Author : andres barrientos
;


; ---------------------- librería ---------------------
.include "M328PDEF.inc"

.org 0x0000			;acá empieza el código
	RJMP	RESET

; --------------------- configuramos el stack ----------------
RESET:
	LDI		R16, HIGH(RAMEND)
	OUT		SPH, R16
	LDI		R16, LOW(RAMEND)
	OUT		SPL,R16

; --------------- seteamos PB0 como salida ---------------------
MAIN:
	LDI		R16, 0x01
	OUT		DDRB, R16

	SBI		PORTB, 0
	CALL	DELAY

	CBI		PORTB, 0
	CALL	DELAY

	RJMP	MAIN

	

; -------------------- delay ------------------------
DELAY: 
	LDI		R17, 50
DELAY1:
	LDI		R18, 255
DELAY2:
	LDI		R19, 255
DELAY_LARGO:
	DEC		R19
	BRNE	DELAY_LARGO
	DEC		R18
	BRNE	DELAY2
	DEC		R17
	BRNE	DELAY1
	RET
