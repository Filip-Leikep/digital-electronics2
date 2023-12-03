#ifndef EEPROM_I2C_H
# define EEPROM_I2C_H

#include <avr/io.h>

void eeprom_write(uint16_t eeprom_i2c_addr, uint16_t addr, uint8_t data);

uint8_t eeprom_read(uint16_t address);


#endif