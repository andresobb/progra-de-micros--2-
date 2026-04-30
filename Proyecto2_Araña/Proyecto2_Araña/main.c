/*
 * Proyecto2_Araña.c
 *
 * Created:4/30/2026 4:26:11 PM 
 * Author: Andrés Barrientos
 * Description: Araña con servomotores
 */
/****************************************/
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "ADC/ADC.h"

/****************************************/
// Function prototypes
void setup(void);
/****************************************/
// Main Function

int main(void)
{
	setup();
	
	while (1)
	{
	}
}

/****************************************/
// NON-Interrupt subroutines
void setup(void)
{
	cli();
	
	ADC_init();
	
	sei();
}

/****************************************/
// Interrupt routines