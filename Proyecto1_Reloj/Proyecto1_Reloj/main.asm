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

.equ T1VALUE = 0xC2F7		//49,911 PARA 1s
.equ M_HORA = 0
.equ M_FECHA = 1
.equ M_CONF_HORA = 2
.equ M_CONF_FECHA = 3
.equ M_CONF_ALARMA = 4

.def TICKS = R18
.def SEG = R20
.def MIN_U = R21
.def MIN_D = R22
.def HOR_U = R23
.def HOR_D = R24
.def DIGIDX = R25
.def DIGITO	= R26
.def SALIDA7 = R27
.def FLAGS = R28		//bit 0 (1 - alarma activa, 0 - inactiva), bit1 si esta seteada o no
.def DIA_U = R17
.def DIA_D = R19
.def MES_U = R29
.def MES_D = R30
.def MODE = R31

.cseg
.org 0x0000
	RJMP	SETUP
.org OVF0addr		//0x0020
	RJMP TIMER0_OVF
.org PCI1addr
	RJMP PORTC_ISR
.org	0x0040
	
 /****************************************/
// Configuración de la pila
SETUP:

segdig: .dB 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x18

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
OUT		DDRD, R16			;DISPLAYS de 7 segmentos
LDI		R16, 0x00
OUT		PORTD, R16

LDI		R16, 0xFF
OUT		DDRB, R16			//PB0-PB3 multiplex y PB4 BUZZER
LDI		R16, 0x00
OUT		PORTB, R16

LDI		R16, 0x38			//0011 1000	; PC0 - PC2 botones, PC3 - P5 LEDS DE ESTADOS
OUT		DDRC, R16
LDI		R16, 0x07			// pullups
OUT		PORTC, R16	

LDI		R16, (1 << PCIE1)
STS		PCICR, R16

LDI		R16, (1 << PCINT10) | (1 << PCINT9) | (1 << PCINT8)
STS		PCMSK1, R16

CLR		SEG
CLR		TICKS
CLR		MIN_U
CLR		MIN_D
CLR		HOR_U
CLR		HOR_D	
CLR		DIA_U
CLR		DIA_D
CLR		MES_U
CLR		MES_D
CLR		DIGIDX
CLR		DIGITO
CLR		SALIDA7
CLR		MODE
CLR		R0

CALL INIT_TMR0

LDI		R16, (1 << TOIE0)
STS		TIMSK0, R16

SEI

    
/****************************************/
// Loop Infinito
MAIN_LOOP:
	RJMP    MAIN_LOOP

/****************************************/
// NON-Interrupt subroutines

INIT_TMR0:		;usamos prescaler de 64, esto sera para el multiplexado
	LDI		R16, (1 << CS01) | (1 << CS00)
	OUT		TCCR0B, R16

	LDI		R16, 178		//aprox 5ms
	OUT		TCNT0, R16

	RET

/****************************************/
// Interrupt routines

TIMER0_OVF:
	PUSH	R16
	PUSH	R15
	PUSH	R17
	PUSH	R30
	PUSH	R31
	IN		R16, SREG
	PUSH	R16
	
	LDI		R16, 178		//aprox 5ms
	OUT		TCNT0, R16

	INC		TICKS
	CPI		TICKS, 200			// 200 * 5ms = 1s
	BRNE	MUX_START
	CLR		TICKS

	SBI		PINB, PINB5

	INC		SEG
	CPI		SEG, 60
	BRNE	MUX_START
	CLR		SEG

	//minutos
	INC		MIN_U
	CPI		MIN_U, 10
	BRNE	MUX_START
	CLR		MIN_U
	INC		MIN_D
	CPI		MIN_D, 6
	BRNE	MUX_START
	CLR		MIN_D

	//horas
	INC		HOR_U
	CPI		HOR_U, 10
	BRNE	FIX_24
	CLR		HOR_U
	INC		HOR_D

	FIX_24:
	CPI		HOR_D, 2
	BRNE	MUX_START
	CPI		HOR_U, 4
	BRNE	MUX_START
	CLR		HOR_D
	CLR		HOR_U

	//hacer aca lo de que llegue a 24h y cambiar dia

MUX_START:

	CBI		PORTB, PORTB0
	CBI		PORTB, PORTB1
	CBI		PORTB, PORTB2
	CBI		PORTB, PORTB3

	CPI		MODE, M_HORA
	BREQ	SHOW_HORA
	RJMP	SHOW_FECHA

SHOW_HORA:
	CPI		DIGIDX, 0
	BREQ	D0
	CPI		DIGIDX, 1
	BREQ	D1
	CPI		DIGIDX, 2
	BREQ	D2
	RJMP	D3		//tipo else

D0:									//decimal hora
	MOV		DIGITO, HOR_D
	SBI		PORTB, PORTB0
	RJMP	SHOW

D1:									//unidad hora
	MOV		DIGITO, HOR_U
	SBI		PORTB, PORTB1
	RJMP	SHOW

D2:									//decimal minuto
	MOV		DIGITO, MIN_D
	SBI		PORTB, PORTB2
	RJMP	SHOW

D3:									//unidad minuto
	MOV		DIGITO, MIN_U
	SBI		PORTB, PORTB3
	RJMP	SHOW

SHOW_FECHA:
	CPI		DIGIDX, 0
	BREQ	F0
	CPI		DIGIDX, 1
	BREQ	F1
	CPI		DIGIDX, 2
	BREQ	F2
	RJMP	F3		//tipo else

F0:									//decimal hora
	MOV		DIGITO, DIA_D
	SBI		PORTB, PORTB0
	RJMP	SHOW

F1:									//unidad hora
	MOV		DIGITO, DIA_U
	SBI		PORTB, PORTB1
	RJMP	SHOW

F2:									//decimal minuto
	MOV		DIGITO, MES_D
	SBI		PORTB, PORTB2
	RJMP	SHOW

F3:									//unidad minuto
	MOV		DIGITO, MES_U
	SBI		PORTB, PORTB3
	RJMP	SHOW


SHOW:
	LDI		ZH, HIGH(segdig << 1)
    LDI		ZL, LOW(segdig << 1)
    ADD		ZL, DIGITO
    ADC		ZH, R0
    LPM		SALIDA7, Z
    OUT		PORTD, SALIDA7
	
	INC		DIGIDX
	CPI		DIGIDX, 4
	BRNE	SALIDA_ISR
	CLR		DIGIDX

SALIDA_ISR:
	POP		R16
	OUT		SREG, R16
	POP		R31
	POP		R30
	POP		R17
	POP		R15
	POP		R16

	RETI

PORTC_ISR:
	PUSH	R16
	IN		R16, SREG
	PUSH	R16
	
	CBI		PORTB, PORTB4		//boton apaga al buzzer

	IN		R16, PINC

	SBRS	R16, 0
	RJMP	BTN0
	RJMP	END_ISR

BTN0:
	LDI		R16, 1
	EOR		MODE, R16

END_ISR:
	POP		R16
	OUT		SREG, R16
	POP		R16

	RETI



/****************************************/
