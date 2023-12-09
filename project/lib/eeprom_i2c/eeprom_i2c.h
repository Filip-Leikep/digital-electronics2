#ifndef EEPROM_I2C_H
# define EEPROM_I2C_H

/***********************************************************************
 * 
 * AT24C32 eeprom library for AVR-GCC.
 * ATmega328P (16 MHz)
 * 
 **********************************************************************/

/**
 * @file
 * @code #include <twi.h> @endcode
 *
 * @brief AT24C32 eeprom library for AVR-GCC.
 *
 * This library defines few functions for using the AT24C32 eeprom on RTC module with ATmega328P. For using this library it is necessary to include twi library from doc. Tomas Friza.
 *
 * @warning It is necessary to include TWI library from doc. Tomas Fryza and initialize twi comunication before using any of the functions in this library.
 * (https://github.com/tomas-fryza/digital-electronics-2/tree/master/labs/06-i2c)
 * @author Vaclav Kubes
 * @{
 */


/*==================INCLUDES==================*/
#include <avr/io.h>


/*==================DEFINING FUNCTIONS==================*/

/**
 * @brief  Write  one byte to given address in eeprom.
 * @param  I2C address of AT24C32 eeprom (usually 0x57)
 * @param  data Byte to be saved in eeprom
 * @param  address of memory cell in eeprom (0 - 4096)
 * @return ACK/NACK received value from eeprom
 * @retval 0 - ACK has been received - writing succesful
 * @retval 1 - NACK has been received - something is wrong
 */
uint8_t eeprom_B_write(uint8_t eeprom_i2c_addr, uint16_t addr, uint8_t data);

/**
 * @brief  Write  up to 32 bytes (one page) to eeprom starting from given address.
 * @param  I2C address of AT24C32 eeprom (usually 0x57)
 * @param  array of Bytes to be saved in eeprom
 * @param  address of the first memory cell in eeprom (0 - 4096) where writing should begin
 * @return ACK/NACK received value from eeprom
 * @retval 0 - ACK has been received - writing succesful
 * @retval 1 - NACK has been received - something is wrong
 * @note When address exceeds page, the address is set to the first cell in that that page (risk of overwriting data)  
 */
uint8_t eeprom_P_write(uint8_t eeprom_i2c_addr, uint16_t addr, uint8_t data[], uint8_t n_bytes);

/**
 * @brief  Read one byte from eeprom from given address.
 * @param  I2C address of AT24C32 eeprom (usually 0x57)
 * @param  address of memory cell in eeprom (0 - 4096)
 * @return Byte read from eeprom memory cell
 */
uint8_t eeprom_B_read(uint8_t eeprom_i2c_addr, uint16_t addr);

/**
 * @brief  Read n memory cells in eeprom starting from given address.
 * @param  I2C address of AT24C32 eeprom (usually 0x57)
 * @param  address of the first memory cell in eeprom (0 - 4096) where reading should begin
 * @param  Number of cells to be read
 * @param  One demensional array for retrieving read data in eeprom. 
 * @return ACK/NACK received value from eeprom
 * @retval 0 - ACK has been received - writing succesful
 * @retval 1 - NACK has been received - something is wrong
 * @note The array where read data are saved must be as global variable with at least the same number of positions as the number of read cells is.  
 */
uint8_t eeprom_read(uint8_t eeprom_i2c_addr, uint16_t start_addr, uint8_t n_bytes, uint8_t *data);

/** @} */

#endif