/*
 * EEPROM.h
 *
 * Created: 5/9/2026 3:54:38 PM
 *  Author: andres barrientos
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

#include <avr/io.h>
#include <stdint.h>

void EEPROM_write(uint16_t direccion, uint8_t dato);
uint8_t EEPROM_read(uint16_t direccion);

#endif /* EEPROM_H_ */