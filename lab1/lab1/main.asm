/*
* lab1.asm
*
* Creado:  05 de febrero de 2026
* Autor : Andres Barrientos
* Descripción: Prelab - Contador de 4 bits con botones
*/
/****************************************/
// Encabezado (Definición de Registros, Variables y Constantes)
.include "M328PDEF.inc"     // Include definitions specific to ATMega328P

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

LDI		R16, 0x0F			//los cuatro bits bajos para salidas del contador
OUT		DDRD, R16
LDI		R16, 0x00
OUT		PORTD, R16

LDI		R16, 0x00
OUT		DDRB, R16
LDI		R16, 0x0F
OUT		PORTB, R16			//habilitamos los primeros cuatro bits del B para los botones, añadimos pull ups


// detalles nt
LDI		R17, 0xFF		//lo usaremos para comparar con el estado de los botones.
LDI		R18, 0x00		//en R18 llevaremos guardado el contador
    
/****************************************/
// Loop Infinito
MAIN_LOOP:

	IN		R16, PINB
	CP		R17, R16
	BREQ	MAIN_LOOP

	CALL	DELAY

	IN		R16, PINB
	CP		R17, R16
	BREQ	MAIN_LOOP

	MOV		R17, R16

	CALL	CONTADOR

	MOV		R19, R18
	OUT		PORTD, R19

	RJMP	MAIN_LOOP
    

/****************************************/
// NON-Interrupt subroutines

CONTADOR:
	SBRS	R16, 0
	INC		R18

	SBRS	R16, 1
	DEC		R18

	ANDI	R18, 0x0F

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