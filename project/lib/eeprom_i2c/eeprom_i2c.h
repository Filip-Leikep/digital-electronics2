#ifndef EEPROM_I2C_H
# define EEPROM_I2C_H

#include <avr/io.h>

void eeprom_write(uint16_t address, uint8_t data);

uint8_t eeprom_read(uint16_t address);


#endif