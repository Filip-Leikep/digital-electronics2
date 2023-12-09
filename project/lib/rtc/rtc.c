/***********************************************************************
 * 
 * RTC module library for AVR-GCC.
 * ATmega328P (16 MHz)
 * 
 **********************************************************************/

/*===========INCLUDES============*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include "rtc.h"
#include <twi.h>
#include <uart.h>
#include <stdlib.h> 

/*==========DEFINES==============*/
#define START_ADDR 00 /**<address where reading and writing begins in RTC*/


/*=============FUNCTIONS=========*/

/**********************************************************************
 * Function: BCD_to_DEC()
 * Purpose:  Convert binary code decimal to decimal (used for reading from RTC registers) - this function is used internally only.
 * Input:    Two digit number in BCD (8 b).
 * Returns:  Converted number in dec.
 **********************************************************************/
uint8_t BCD_to_DEC (uint8_t BCD){
    uint8_t units;
    uint8_t tens;
    units = BCD;
    units = units & 0b00001111; //units are represented by lower nibble 
    tens = BCD >> 4; //tens are represented by higher nibble 
    return (units + 10 * tens); //return converted value
}

/**********************************************************************
 * Function: DEC_to_BCD()
 * Purpose:  Convert decimal to binary code decimal (used for writing to RTC registers) - this function is used internally only.
 * Input:    Two digit number in decimal (8 b).
 * Returns:  Converted number in BCD.
 **********************************************************************/
uint8_t DEC_to_BCD (uint8_t DEC){
    uint8_t upper_nibble = DEC / 10; //extract number of tens
    uint8_t lower_nibble = DEC % 10; //extract number of units
    upper_nibble = upper_nibble << 4; //tens are represented by upper nibble
    return ((upper_nibble & 0b11110000) | (lower_nibble & 0b00001111)); //return BCD number
}

/**********************************************************************
 * Function: RTC_init()
 * Purpose:  Function for setting currnet time and date
 * Input:    I2C address of RTC module
 * Input:    Time and date to be set. In order: seconds, minutes, hours, day, date, month, year
 * Returns:  void
 **********************************************************************/
void RTC_init(uint8_t rtc_i2c_address, uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t day, uint8_t date, uint8_t month, uint8_t year){
    twi_start();
     if (twi_write((rtc_i2c_address<<1) | TWI_WRITE) == 0) { //if rtc module is present at i2c bus set current date time
        twi_write(START_ADDR);
        twi_write(DEC_to_BCD(seconds));
        twi_write(DEC_to_BCD(minutes));
        twi_write(DEC_to_BCD(hours));
        twi_write(DEC_to_BCD(day));
        twi_write(DEC_to_BCD(date));
        twi_write(DEC_to_BCD(month));
        twi_write(DEC_to_BCD(year));
     }
     twi_stop();
}

/**********************************************************************
 * Function: RTC_now()
 * Purpose:  Function to obtain selected time or date value
 * Input:    I2C address of RTC module
 * Input:    Index of required data ([0] seconds, [1] minutes, [2] hours, [3] day, [4] date, [5] month, [6] year)
 * Returns:  Current time/date value 
 **********************************************************************/
uint8_t RTC_now(uint8_t rtc_i2c_address, uint8_t data){
    uint8_t time [7]; 
    twi_start();
    if (twi_write((rtc_i2c_address<<1) | TWI_WRITE) == 0) { //if rtc module is present at i2c bus read current date and time 
        twi_write(START_ADDR);
        twi_stop();
        twi_start();
        twi_write((rtc_i2c_address<<1) | TWI_READ);
        time[0] = BCD_to_DEC(twi_read(TWI_ACK));
        time[1] = BCD_to_DEC(twi_read(TWI_ACK));
        time[2] = BCD_to_DEC(twi_read(TWI_ACK));
        time[3] = BCD_to_DEC(twi_read(TWI_ACK));
        time[4] = BCD_to_DEC(twi_read(TWI_ACK));
        time[5] = BCD_to_DEC(twi_read(TWI_ACK));
        time[6] = BCD_to_DEC(twi_read(TWI_NACK));
    }
    twi_stop();
    switch(data){ //depending on the desired output return time or date value
        case 0:
            return time[0];
            break;
        case 1:
            return time[1];
            break;

        case 2:
            return time[2];
            break;

        case 3:
            return time[3];
            break;

        case 4:
            return time[4];
            break;

        case 5:
            return time[5];
            break;

        case 6:
            return time[6];
            break;
    }  
    return 0; 
}