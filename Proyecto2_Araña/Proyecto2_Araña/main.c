/*
 * Proyecto2_Araña.c
 *
 * Created: 4/30/2026 3:24:18 PM
 * Author: Andrés Barrientos
 * Description: Araña con servomotores
 */
/****************************************/
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "ADC/ADC.h"
#include "PWM1/PWM1.h"
#include "PWM0/PWM0.h"
#include "PWM2/PWM2.h"
#include "UART/UART.h"
#include "EEPROM/EEPROM.h"

/****************************************/
// Function prototypes
void setup(void);
void ADC_init(void);

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
	
	/*PWM1_init();
	PWM2_init();
	PWM0_init();*/
	
	sei();
}

/****************************************/
// Interrupt routines
