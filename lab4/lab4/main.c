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
#include <stdint.h>

/****************************************/
// Function prototypes
void setup(void);
void timer0_init(void);
void adc_init(void);
uint8_t leer_ADC(void);
void comparar_valor(void);

// variables
volatile uint8_t contador = 0;
volatile uint16_t antirebote = 0;
volatile uint8_t valor_ADC = 0;
volatile uint8_t display_actual = 0;		//0 - display 1; 1 - display 2; 2 - LEDs

uint8_t tabla7seg[] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x18, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E};

/****************************************/
// Main Function
int main(void)
{
	setup();
	
	while (1)
	{
		valor_ADC = leer_ADC();
		comparar_valor();
	}
	
}

/****************************************/
// NON-Interrupt subroutines
void setup(void)
{
	cli();
	
	//puerto D como salida
	DDRD = 0xFF;
	PORTD = 0x00;
	
	DDRB |= ((1 << PORTB3) | (1 << PORTB2) | (1 << PORTB1) | (1 << PORTB0));
	PORTB &= ~((1 << PORTB3) | (1 << PORTB2) | (1 << PORTB1) | (1 << PORTB0));		//todo apagados con 0

	//PC1 Y 2 como entradas y pull ups, PC0 es el ADC0
	DDRC &= ~((1 << PORTC2) | (1 << PORTC1) | (1 << PORTC0));
	PORTC |= ((1 << PORTC2)| (1 << PORTC1));

	//interrupciones en el puerto C
	// PC1 = PCINT9, PC2 = PCINT10
	PCMSK1 |= ((1 << PCINT10) | (1 << PCINT9));
	PCICR |= (1 << PCIE1);
	
	timer0_init();
	adc_init();
	
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

void adc_init(void)
{
	ADMUX |= ((1 << REFS0) | (1 << ADLAR));		//adlar 1 justifica a la izquierda, refs0 pone de referencia a AVcc
	ADCSRA |= ((1 << ADEN) | (1 << ADPS2)| (1 << ADPS1) | (1 << ADPS0));		//prescaler 128 --> 125kHz PERO POR QUE
	DIDR0 = (1 << ADC0D);
}

uint8_t leer_ADC(void)
{
	ADCSRA |= (1 << ADSC);		//iniciar conversion
	while (ADCSRA & (1 << ADSC));
	return ADCH;	
}

void comparar_valor(void)
{
	if (valor_ADC > contador)
	{
		PORTB |= (1 << PORTB3);
	}
	else 
	{
		PORTB &= ~(1 << PORTB3);
	}
}

/****************************************/
// Interrupt routines
ISR(PCINT1_vect)
{
	if (antirebote == 0)
	{
		if (!(PINC & (1 << PORTC2)))
		{
			contador++;
			antirebote = 250;		
		}
		else if(!(PINC & (1 << PORTC1)))
		{
			contador--;
			antirebote = 250;
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

	//empezamos el mux
	//apagamos todo primero
	PORTB &= ~((1 << PORTB2) | (1 << PORTB1) | (1 << PORTB0));		//todo apagados con 0

	if (display_actual == 0){
	PORTD = tabla7seg[valor_ADC & 0x0F];		//nibble bajo
	PORTB |= (1 << PORTB0);					//display 1 encendido
	display_actual = 1;
	}
	
	else if (display_actual == 1){
	PORTD = tabla7seg[(valor_ADC >> 4) & 0x0F];		//nibble alto
	PORTB |= (1 << PORTB1);						//display 2 enncendido
	display_actual = 2;
	}

	else {
	PORTD = contador;
	PORTB |= (1 << PORTB2);			//leds encendidos supuestamente lol
	display_actual = 0;
	}
}

