
#include <avr/io.h>
#include <avr/interrupt.h>
#include "rtc.h"
#include <twi.h>

//address where reading and writing begins in RTC
#define START_ADDR 00 


//function for setting currnet time and date
void RTC_init(uint8_t rtc_i2c_address, uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t day, uint8_t date, uint8_t month, uint8_t year){
    twi_start();
     if (twi_write((rtc_i2c_address<<1) | TWI_WRITE) == 0) {
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

//function for getting current time and date form RTC module
uint8_t RTC_now(uint8_t rtc_i2c_address){
    uint8_t time [7]; 
    twi_start();
    if (twi_write((rtc_i2c_address<<1) | TWI_WRITE) == 0) {
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
        time[6] = BCD_to_DEC(twi_read(TWI_ACK));
        time[7] = BCD_to_DEC(twi_read(TWI_NACK));
    }
    twi_stop();
    
    return time;
}

//convert binary code decimal to decimal (used for reading from RTC registers)
uint8_t BCD_to_DEC (uint8_t BCD){
    uint8_t units;
    uint8_t tens;
    units = BCD & 0b00001111;
    tens = BCD & 0b11110000;
    return (units + 10 * tens);
}

//convert decimal to binary code decimal (used for writing to RTC registers)
uint8_t DEC_to_BCD (uint8_t DEC){
    uint8_t upper_nibble = DEC / 10;
    uint8_t lower_nibble = DEC % 10;
    return (upper_nibble | lower_nibble);
}