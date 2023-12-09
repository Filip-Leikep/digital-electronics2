/***********************************************************************
 * 
 * AT24C32 eeprom library for AVR-GCC.
 * ATmega328P (16 MHz)
 * 
 **********************************************************************/


/*===============INCLUDES================*/
#include <eeprom_i2c.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <twi.h>
#include <stdlib.h> 

/*===============DEFINES================*/
#define WAIT_WRITE 160000 /**< @brief To give some time to write data from eeprom buffer to actual nonvolatile memory (valid only for 16 MHz)*/



/*===============FUNCTIONS============*/

/**********************************************************************
 * Function: eeprom_B_write()
 * Purpose:  Write one byte into eeprom at given memory address.
 * Input:    I2C address of AT24C32 eeprom chip (usually 0x57)
 * Input:    data Byte to be saved in eeprom
 * Input:    address of memory cell in eeprom (0 - 4096)
 * Returns:  ACK/NACK from I2C communication with eeprom.
 **********************************************************************/
uint8_t eeprom_B_write(uint8_t eeprom_i2c_addr, uint16_t addr, uint8_t data){
    uint8_t status;
    uint8_t second_word_addr = addr & 0xFF; //spliting 16 b memory address into to 8 b variables to send via I2C to eeprom
    uint8_t first_word_addr = (addr >> 8) & 0xFF;
    twi_start();
    status = twi_write((eeprom_i2c_addr<<1) | TWI_WRITE); 
    if (status == 0) { //if eeprom is present on I2C bus then it starts writing
        twi_write(first_word_addr); //first addresing the memory cell to write there
        twi_write(second_word_addr);
        status = twi_write(data); //writing 8 b to memory cell
    }
    twi_stop();
    for(uint32_t i = 0; i < WAIT_WRITE; i++) asm("NOP"); //wait some time to give the eeprom chance to save the written datata to nonvolatile memory
    return status; //retrn status of I2C communication with eeprom
}

/**********************************************************************
 * Function: eeprom_P_write()
 * Purpose:  Write up to 32 bytes into eeprom starting at given memory address.
 * Input:    I2C address of AT24C32 eeprom (usually 0x57)
 * Input:    array of Bytes to be saved in eeprom
 * Input:    address of the first memory cell in eeprom (0 - 4096) where writing should begin
 * Returns:  ACK/NACK from I2C communication with eeprom.
 **********************************************************************/
uint8_t eeprom_P_write(uint8_t eeprom_i2c_addr, uint16_t addr, uint8_t data[], uint8_t n_bytes){
    uint8_t status;
    uint8_t second_word_addr = addr & 0xFF; //spliting 16 b memory address into to 8 b variables to send via I2C to eeprom
    uint8_t first_word_addr = (addr >> 8) & 0xFF;
    twi_start();
    if (twi_write((eeprom_i2c_addr<<1) | TWI_WRITE) == 0) { //if eeprom is present on I2C bus then it starts writing
        twi_write(first_word_addr); //first addresing the memory cell to write there
        twi_write(second_word_addr);
        for (uint8_t i = 0; i < n_bytes; i++){ //as long as there are data to be saved write them to eeprom
            status = twi_write(data[i]);
        }
    }
    twi_stop();
    for(uint32_t i = 0; i < WAIT_WRITE; i++) asm("NOP"); //wait some time to give the eeprom chance to save the written datata to nonvolatile memory
    return status; //retrn status of I2C communication with eeprom
}

/**********************************************************************
 * Function: eeprom_B_read()
 * Purpose:  Read one byte from eeprom from given address.
 * Input:    I2C address of AT24C32 eeprom (usually 0x57)
 * Input:    address of the memory cell in eeprom (0 - 4096)
 * Returns:  Byte read from eeprom memory cell
 **********************************************************************/
uint8_t eeprom_B_read(uint8_t eeprom_i2c_addr, uint16_t addr){
    uint8_t second_word_addr = addr & 0xFF; //spliting 16 b memory address into to 8 b variables to send it via I2C to eeprom
    uint8_t first_word_addr = (addr >> 8); 
    uint8_t data;
    twi_start();
    if (twi_write((eeprom_i2c_addr<<1) | TWI_WRITE) == 0) { //if eeprom is present on I2C bus then it starts reading
        twi_write(first_word_addr); //first addresing the memory cell to read from there
        twi_write(second_word_addr);
        twi_start();
        twi_write((eeprom_i2c_addr<<1) | TWI_READ);
        //data = 55;
        data = twi_read(TWI_NACK); //read 1 B from eeprom memory cell
    }
    twi_stop();
    return data; //return obtained 1 Byte from eeprom memory cell with given address
}

/**********************************************************************
 * Function: eeprom_read()
 * Purpose:  Read n memory cells in eeprom starting from given address.
 * Input:    I2C address of AT24C32 eeprom (usually 0x57)
 * Input:    address of the memory cell in eeprom (0 - 4096) where reading should begin
 * Input:    Number of cells to be read
 * Input:    One demensional array for retrieving read data in eeprom
 * Returns:  ACK/NACK received value from i2C communication with eeprom
 **********************************************************************/
uint8_t eeprom_read(uint8_t eeprom_i2c_addr, uint16_t start_addr, uint8_t n_bytes, uint8_t *data){
    uint8_t status;
    uint8_t second_word_addr = start_addr & 0xFF; //spliting 16 b memory address into to 8 b variables to send it via I2C to eeprom
    uint8_t first_word_addr = (start_addr >> 8);
    twi_start();
    if (twi_write((eeprom_i2c_addr<<1) | TWI_WRITE) == 0) { //if eeprom is present on I2C bus then it starts reading
        twi_write(first_word_addr); //first addresing the memory cell to read from there
        twi_write(second_word_addr);
        twi_start();
        status = twi_write((eeprom_i2c_addr<<1) | TWI_READ);
        for(uint8_t i = 0; i < n_bytes; i++){ //as long as there are data to be read save them to array
            if(i == (n_bytes - 1)){
                data[i] = twi_read(TWI_NACK);
                status = 0;
            }else{
                data[i] = twi_read(TWI_ACK);
            }   
        }
    }
    twi_stop();

    return status; //retrn status of I2C communication with eeprom
}