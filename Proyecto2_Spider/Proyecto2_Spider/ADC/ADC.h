/*
 * ADC.h
 *
 * Created: 4/30/2026 4:44:40 PM
 *  Author: andres barrientos
 */ 


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

void ADC_init(void);

extern volatile uint16_t ADC_valores[6];

#endif /* ADC_H_ */