/*
NombreProgra.asm
*
* Creado: 21-Jan-23
* Autor : Andrés Barrientos
* Descripción: Vamos a aprender a debuggear xd
*/
/****************************************/
// Encabezado (Definición de Registros, Variables y Constantes)
.include "M328PDEF.inc"     // Include definitions specific to ATMega328P
.dseg
.org    SRAM_START
//variable_name:     .byte   1   // Memory alocation for variable_name:     .byte   (byte size)

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
SETUP:
	LDI		R16, 0x00
    
/****************************************/
// Loop Infinito
MAIN_LOOP:
	INC		R16
	CALL	SUMAR_R16
    RJMP    MAIN_LOOP

/****************************************/
// NON-Interrupt subroutines

SUMAR_R16:
	LDI		R17, 0x05
	ADD		R16, R17	
	RET

/****************************************/
// Interrupt routines

/****************************************/