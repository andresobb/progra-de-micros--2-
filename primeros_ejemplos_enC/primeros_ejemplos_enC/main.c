/*
 * primeros_ejemplos_enC.c
 *
 * Created: 3/11/2026 4:46:42 PM
 * Author: Andres Barrientos
 * Description: Ejemplos en clase
 */
/****************************************/
// Encabezado (Libraries)

#include <avr/io.h>
#include <AVR/interrupt.h>	//para interrupciones
#include <stdint.h>		//para variables

#define	TCNT0_value 100
uint8_t counter = 0;

/****************************************/
// Function prototypes
void setup();
void initTMR0();

/****************************************/
// Main Function
int main(void)
{
	cli();
	setup();
	//Habilitar interrupcion de OVF0
	TIMSK0 |= (1 << TOIE0);
	sei();
	
	while(1)
	{
		
	}	
}

/****************************************/
// NON-Interrupt subroutines
void setup()
{
	//FREQ RELOJ = 16MHZ / 16 = 1MHZ
	CLKPR	= (1 << CLKPCE);
	CLKPR	= (1 << CLKPS2);
	
	//PUERTOS
	DDRC	= 0x0F;
	PORTC	= 0x00;
	
	initTMR0();
		
}
void initTMR0()
{
	TCCR0A &= ~((1 << WGM01) | (1 << WGM00));		//modo normal
	TCCR0B &= ~(1 << WGM02);	
	
	//PRESCALER 64
	TCCR0B &= ~(1 << CS02);
	TCCR0B |= ((1 << CS01) | (1 << CS00));
	
	//CARGAMOS TCNT0
	TCNT0 = TCNT0_value;
	
}

/****************************************/
// Interrupt routines
ISR(TIMER0_OVF_vect)
{
	TCNT0 = TCNT0_value;
	
	counter++;  // lo mismo a counter = counter + 1;
	
	if (counter == 50)
	{
		counter = 0;
		PORTC++;
		PORTC &= 0x0F;		// mascara asi como el ANDI	0x0F
	}
	
}
