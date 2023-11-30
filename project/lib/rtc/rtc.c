
#include <avr/io.h>
#include <avr/interrupt.h>
#include "rtc.h"
#include <twi.h>
#include <uart.h>
#include <stdlib.h> 

//address where reading and writing begins in RTC
#define START_ADDR 00 

//convert binary code decimal to decimal (used for reading from RTC registers)
uint8_t BCD_to_DEC (uint8_t BCD){
    uint8_t units;
    uint8_t tens;
    //units = BCD & 0b00001111;
    units = BCD;
    units = units & 0b00001111;
    //char string[8];
    //itoa(units, string, 10);
    //uart_puts("jednotky: ");
   // uart_puts(string);
   // uart_putc('\n');
    tens = BCD >> 4;
    //itoa(tens, string, 10);
    //uart_puts("desítky: ");
    //uart_puts(string);
    //uart_putc('\n');
    //itoa(units + 10 * tens, string, 10);
    //uart_puts("výsledek: ");
    //uart_puts(string);
    //uart_putc('\n');
    return (units + 10 * tens);
}

//convert decimal to binary code decimal (used for writing to RTC registers)
uint8_t DEC_to_BCD (uint8_t DEC){
    char string [4];
    uint8_t upper_nibble = DEC / 10;
    //itoa(upper_nibble, string, 2);
    //uart_puts("BCD: ");
    //uart_puts(string);
    
    //uart_puts("výsledek: ");
    //uart_puts(string);
    
    uint8_t lower_nibble = DEC % 10;
    //itoa(lower_nibble, string, 2);
    //uart_puts("desítky: ");
    //uart_puts(string);
    //uart_putc('\n');
    upper_nibble = upper_nibble << 4;
    return ((upper_nibble & 0b11110000) | (lower_nibble & 0b00001111));
}

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
/*
//function for getting current time and date form RTC module
uint8_t *RTC_now(uint8_t rtc_i2c_address){
    static uint8_t time [7]; 
    //uint8_t time1;
    //uint8_t time2;
    //uint8_t seconds;
    //uint8_t minutes;
    //uint8_t hour;
    //uint8_t day;
    //uint8_t date;
    //uint8_t month;
    //uint8_t year;
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
       time[6] = BCD_to_DEC(twi_read(TWI_NACK));
        //time[7] = BCD_to_DEC(twi_read(TWI_NACK));
    }
    twi_stop();
    
    return time;
}
*/

uint8_t RTC_now(uint8_t rtc_i2c_address, uint8_t data){
    uint8_t time [7]; 
    //uint8_t time1;
    //uint8_t time2;
    /*uint8_t seconds;
    uint8_t minutes;
    uint8_t hour;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint8_t year;*/
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
       time[6] = BCD_to_DEC(twi_read(TWI_NACK));
        //time[7] = BCD_to_DEC(twi_read(TWI_NACK));
    }
    twi_stop();
    
    switch(data){
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
}