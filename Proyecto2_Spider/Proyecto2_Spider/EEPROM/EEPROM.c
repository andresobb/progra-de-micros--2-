/*
 * EEPROM.c
 *
 * Created: 5/9/2026 3:54:28 PM
 *  Author: andres barrientos
 */ 

#include "EEPROM.h"

void EEPROM_write(uint16_t direccion, uint8_t dato)
{
	while (EECR & (1 << EEPE));			//esperamos a que termine cualquier escritura anterior
	
	EEAR = direccion;		//cargamos direccion
	
	EEDR = dato;			//cargamos dato
	
	EECR |= (1 << EEMPE);		//habilitamos escritura maestra
	
	EECR |= (1 << EEPE);		//iniciamos escritura
	
}

uint8_t EEPROM_read(uint16_t direccion)
{
	while (EECR & (1 << EEPE));		//esperamos
	
	EEAR = direccion;
	
	EECR |= (1 << EERE);
	
	return EEDR;
}