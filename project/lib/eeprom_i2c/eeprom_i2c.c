
#include <eeprom_i2c.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <twi.h>
//#include <uart.h>
#include <stdlib.h> 

void eeprom_B_write(uint16_t eeprom_i2c_addr, uint16_t addr, uint8_t data){
    uint8_t first_word_addr = addr & 0x00FF;
    uint8_t second_word_addr = (addr >> 8) & 0x00FF;
    twi_start();
    if (twi_write((eeprom_i2c_addr<<1) | TWI_WRITE) == 0) {
        twi_write(first_word_addr);
        twi_write(second_word_addr);
        twi_write(data);
    }
    twi_stop();
}

void eeprom_P_write(uint16_t eeprom_i2c_addr, uint16_t addr, uint8_t data){
    uint8_t first_word_addr = addr & 0x00FF;
    uint8_t second_word_addr = (addr >> 8) & 0x00FF;
    twi_start();
    if (twi_write((eeprom_i2c_addr<<1) | TWI_WRITE) == 0) {
        twi_write(first_word_addr);
        twi_write(second_word_addr);
    }
}