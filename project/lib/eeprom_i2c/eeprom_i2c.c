
#include <eeprom_i2c.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <twi.h>
//#include <uart.h>
#include <stdlib.h> 

uint8_t eeprom_B_write(uint8_t eeprom_i2c_addr, uint16_t addr, uint8_t data){
    uint8_t status;
    uint8_t second_word_addr = addr & 0xFF;
    uint8_t first_word_addr = (addr >> 8) & 0xFF;
    twi_start();
    status = twi_write((eeprom_i2c_addr<<1) | TWI_WRITE);
    if (status == 0) {
        twi_write(first_word_addr);
        twi_write(second_word_addr);
        status = twi_write(data);
    }
    twi_stop();
    return status;
}

uint8_t eeprom_P_write(uint8_t eeprom_i2c_addr, uint16_t addr, uint8_t data[], uint8_t n_bytes){
    uint8_t status;
    uint8_t second_word_addr = addr & 0xFF;
    uint8_t first_word_addr = (addr >> 8) & 0xFF;
    twi_start();
    if (twi_write((eeprom_i2c_addr<<1) | TWI_WRITE) == 0) {
        twi_write(first_word_addr);
        twi_write(second_word_addr);
        for (uint8_t i = 0; i < n_bytes; i++){
            status = twi_write(data[i]);
        }
    }
    twi_stop();
    return status;
}


uint8_t eeprom_B_read(uint8_t eeprom_i2c_addr, uint16_t addr){
    uint8_t second_word_addr = addr & 0xFF;
    uint8_t first_word_addr = (addr >> 8); //& 0x00FF;
    uint8_t data;// = 100;
    twi_start();
    if (twi_write((eeprom_i2c_addr<<1) | TWI_WRITE) == 0) {
        twi_write(first_word_addr);
        twi_write(second_word_addr);
        twi_start();
        twi_write((eeprom_i2c_addr<<1) | TWI_READ);
        //data = 55;
        data = twi_read(TWI_NACK);
    }
    twi_stop();
    return data;
}


uint8_t eeprom_read(uint8_t eeprom_i2c_addr, uint16_t start_addr, uint8_t n_bytes, uint8_t *data){
    uint8_t status;
    uint8_t second_word_addr = start_addr & 0xFF;
    uint8_t first_word_addr = (start_addr >> 8);// & 0xFF;
    //uint8_t data;
    twi_start();
    if (twi_write((eeprom_i2c_addr<<1) | TWI_WRITE) == 0) {
        twi_write(first_word_addr);
        twi_write(second_word_addr);
        //twi_stop();
        twi_start();
        status = twi_write((eeprom_i2c_addr<<1) | TWI_READ);
        for(uint8_t i = 0; i < n_bytes; i++){
            //status = i;
            if(i == (n_bytes - 1)){
                data[i] = twi_read(TWI_NACK);
                status = 0;
            }else{
                data[i] = twi_read(TWI_ACK);
                //status = i;
            }
            
        }
    }
    twi_stop();
    return status;
}