#ifndef EEPROM_I2C_H
# define EEPROM_I2C_H

#include <avr/io.h>

uint8_t eeprom_B_write(uint8_t eeprom_i2c_addr, uint16_t addr, uint8_t data);
uint8_t eeprom_P_write(uint8_t eeprom_i2c_addr, uint16_t addr, uint8_t data[], uint8_t n_bytes);

uint8_t eeprom_B_read(uint8_t eeprom_i2c_addr, uint16_t addr);
uint8_t eeprom_read(uint8_t eeprom_i2c_addr, uint16_t start_addr, uint8_t n_bytes, uint8_t *data);

#endif