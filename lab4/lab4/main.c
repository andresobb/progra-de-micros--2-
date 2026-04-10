/*
 * lab4.c
 *
 * Created: 4/10/2026 12:40:13 PM
 * Author: Andrés Barrientos
 * Description: Prelab 4 - Contador de 8 bits (C)
 */
/****************************************/
// Encabezado (Libraries)
#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define DEBOUNCE_TIME 20

/****************************************/
// Function prototypes
void setup(void);
void timer0_init(void);

// variables
volatile uint8_t contador = 0;
volatile uint8_t antirebote = 0;

/****************************************/
// Main Function
int main(void)
{
	setup();
	
	while (1)
	{
		PORTD = contador;
	}
	
}

/****************************************/
// NON-Interrupt subroutines
void setup(void)
{
	cli();
	
	//puerto D como salida
	DDRD = 0xFF;
	PORTD = 0x00
	
	//PC1 Y 2 como entradas y pull ups
	DDRC &= ~((1 << PORTC2) | (PORTC1));
	PORTC |= ((1 << PORTC2)| (1 << PORTC1));
	
	//interrupciones en el puerto C
	// PC1 = PCINT9, PC2 = PCINT10
	PCMSK1 |= ((1 << PCINT10) | (1 << PCINT9));
	PCICR |= (1 << PCIE1);
	
	timer0_init();
	
	sei();
}
void timer0_init(void)
{
	TCCR0A = 0x00;
	TCCR0B = 0x00;			//modo normal
	
	TCNT0 = 6;		//precarga para 1ms
	
	//interrupcion por overflow
	TIMSK0 |= (1 << TOIE0);
	
	//prescaler 64
	TCCR0B |= ((1 << CS01) | (1 << CS00));
}

/****************************************/
// Interrupt routines
ISR(PCINT1_vect)
{
	if (antirebote == 0)
	{
		if (!(PINC & (1 << PORTC1)))
		{
			contador++;
			antirebote = 20;		//1ms * 20 
		}
		else if(!(PINC & (1 << PORTC2)))
		{
			contador--;
			antirebote = 20;
		}
	}
}

ISR(TIMER0_OVF_vect)
{
	TCNT0 = 6;
	
	if (antirebote > 0)
	{
		antirebote--;
	}
}


