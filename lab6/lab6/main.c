/*
 * lab6.c
 *
 * Created: 4/23/2026 12:25:32 PM
 * Author: Andrés Barrientos
 * Description: 
 */
/****************************************/
// Encabezado (Libraries)
#define F_CPU 16000000
#include <avr/io.h>	
#include <stdint.h>
#include <avr/interrupt.h>

/****************************************/
// Function prototypes
void setup(void);
void UART_init(void);
void UART_sendChar(char caracter);
char UART_readChar(void);
void mostrarASCII(uint8_t dato);
void cadena(char txt[]);
void ADC_init(void);
void mostrarMenu(void);
uint8_t ADC_read(uint8_t canal);
void enviar_numero(uint8_t valor);

volatile uint8_t received_RX = 0;
volatile uint8_t dato_nuevo = 0;
volatile uint8_t modo_ascii = 0;

/****************************************/
// Main Function
int main(void)
{
	setup();
	
	// parte 1: enviar un caracter una vez al encender
	//UART_sendChar('A'); //(prelab)
	//cadena("Pongan drop dead gracias"); //(lab)
	
	mostrarMenu();
	
	while (1)
	{
		/*paete 2: recibir un caracter y mostrarlo en los leds
		char recibido = UART_readChar();
		mostrarASCII((uint8_t)recibido);*/ //esto era main del prelab
		
		if (dato_nuevo)
		{
			dato_nuevo = 0;			//reseteamos la bandera
			if (modo_ascii == 0)
			{
				if (received_RX == '1')
				{
					uint8_t valor_ADC = ADC_read(0);
					mostrarASCII(valor_ADC);
					
					cadena("\r\n El valor del potenciómetro (0 - 255) es: \r\n");
					enviar_numero(valor_ADC);
					cadena("\r\n");
					
					mostrarMenu();
				}
				else if (received_RX == '2')
				{
					modo_ascii = 1;
					cadena("\r\n Ingrese un caracter: \r\n");
				}
				else
				{
					cadena("\r\n Opción no válida\r\n");
					mostrarMenu();
				}
			}
			else
			{
				mostrarASCII(received_RX);
				
				cadena("\r\n Caracter recibido: ");
				UART_sendChar(received_RX);
				cadena("\r\n");
			
				modo_ascii = 0;
				mostrarMenu();
			}
		}
				
	}
}

/****************************************/
// NON-Interrupt subroutines
void setup(void)
{
	DDRB |= 0x3F;		// PB0-PB5 como salida
	DDRD |= ((1 << DDD6) | (1 << DDD7));		// PD6 Y PD7
	
	PORTB &= ~0x3F;
	PORTD &= ~((1 << PORTD6) | (1 << PORTD7));
	
	cli();
	ADC_init();
	sei();
	
	UART_init();
}

void UART_init(void)
{
	
	// 9600 baudios para 16MHz
	UBRR0H = 0;
	UBRR0L = 103;
	
	UCSR0A = 0;
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);		//habilitamos la transmision y recepcion, interrupcion al recibir dato
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);		//datos de 8 bits
}

void UART_sendChar(char caracter)
{
	while (!(UCSR0A & (1 << UDRE0)));			//espera hasta que el registro de transmision este listo
	UDR0 = caracter;
}

char UART_readChar(void)
{
	while (!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

void cadena(char txt[])
{
	uint8_t i=0;
	
	while (txt[i] != '\0')
	{
		UART_sendChar(txt[i]);
		i++;
	}
	
	
}

void mostrarASCII(uint8_t dato)
{
	// bits 0-5 a PB0-PB5
	PORTB = (PORTB & 0xC0) | (dato & 0x3F);
	
	// bits 6-7 a PD6-PD7
	PORTD = (PORTD & 0x3F) | (dato & 0xC0);
}

void ADC_init(void)
{
	ADMUX |= (1 << REFS0);			// ref avcc
	ADCSRA |= ((1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0));		//habilita adc, prescaler 128
}

void mostrarMenu(void)
{
	cadena("Elija una opción:\r\n");
	cadena("1 - Leer potenciómetro\r\n");
	cadena("2 - Enviar ASCII\r\n");
}

uint8_t ADC_read(uint8_t canal)
{
	ADMUX = (ADMUX & 0xF0) | (canal & 0x0F);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC >> 2;			//solo 8 bits
}

void enviar_numero(uint8_t valor)
{
	if (valor >= 100)
	{
		UART_sendChar('0' + (valor/100));
	}
	if (valor >= 10)
	{
		UART_sendChar('0' + ((valor / 10) % 10));
	}
	UART_sendChar('0' + (valor % 10)); //entender bien esto 
}

/****************************************/
// Interrupt routines
ISR(USART_RX_vect)
{
	received_RX = UDR0;
	dato_nuevo = 1;			//lo usamos como bandera
}