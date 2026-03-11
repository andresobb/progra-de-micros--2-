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

/****************************************/
// Function prototypes
void setup();

/****************************************/
// Main Function
int main(void)
{
	setup();
	while(1)
	{
		delay();
		PORTC ^= ((1 << PORTC3) | (1 << PORTC2) | (1 << PORTC1) | (1 << PORTC0));		// ^= para togglear
	}
}

/****************************************/
// NON-Interrupt subroutines
void setup()
{
		CLKPR	= (1 << CLKPCE);
		CLKPR	= (1 << CLKPS2);
		
		//PODRIA SER TAMBIEN: DDRC = 0xFF; si queremos TODOS los pines
		DDRC |= ((1 << DDC3) | (1 << DDC2) | (1 << DDC1) | (1 << DDC0));		
		PORTC = 0x00;
}

void delay()
{
	for (volatile uint8_t i = 0; i < 255, i++)
	{
		for (volatile uint8_t j = 0; j < 255, j++)
	}
}
/****************************************/
// Interrupt routines
