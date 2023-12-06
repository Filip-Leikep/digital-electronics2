/***********************************************************************
 * 
 * Use Analog-to-digital conversion to read push buttons on LCD keypad
 * shield and display it on LCD screen.
 * 
 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO
 *
 * Copyright (c) 2018 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/


/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <uart.h>            
#include <stdlib.h>         // C library. Needed for number conversions
#include <moisture_sens.h>
#include <oled.h>
#include <gpio.h>
#include <rtc.h>
#include <eeprom_i2c.h>

#define SENSOR_ADR 0x5c
#define EEPROM_ADR 0x57
#define RTC_ADR 0x68
#define N_I2C_DEVICES 3

#define SENSOR_HUM_MEM 0
#define SENSOR_TEMP_MEM 2
#define SENSOR_CHECKSUM 4

#define RELE PD2 

volatile uint8_t new_air_data = 0;
volatile uint8_t new_time_data = 0;
volatile uint8_t water = 0;
volatile uint8_t time_m [7];
uint16_t moist_value;
uint8_t wr = 0;
uint8_t data_r [4];

volatile uint16_t addr = 0;

struct DHT_values_structure {
   uint8_t hum_int;
   uint8_t hum_dec;
   uint8_t temp_int;
   uint8_t temp_dec;
   uint8_t checksum;
};

struct DHT_values_structure dht12;
/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Use Timer/Counter1 and start ADC conversion every 100 ms.
 *           When AD conversion ends, send converted value to LCD screen.
 * Returns:  none
 **********************************************************************/
int main(void)
{
    twi_init(); //inicializace i2c
    RTC_init(RTC_ADR,59,52,15,3,29,11,23);
    // Initialize display
    uart_init(UART_BAUD_SELECT(115200, F_CPU));
    char stringA[2];  // String for converting numbers by itoa()
    uart_puts("test");
    // Configure Analog-to-Digital Convertion unit
    // Select ADC voltage reference to "AVcc with external capacitor at AREF pin"
    moisture_sens_init(); //inicializace půdní vlhkosti
    // Configure 16-bit Timer/Counter1 to start ADC conversion
    // Set prescaler to 33 ms and enable overflow interrupt
    TIM1_OVF_1SEC
    TIM1_OVF_ENABLE
    // Enables interrupts by setting the global interrupt mask
    sei();
    GPIO_mode_output(&DDRD, RELE);

    //pozice na displeji pro vzdusnou teplotu a vlhkost
    display_text();
    
    // Test if devices on I2C are ready
    uint8_t addresses = {SENSOR_ADR, RTC_ADR, EEPROM_ADR};
    twi_test_devices(addresses, N_I2C_DEVICES);


    // Infinite loop
    while (1)
    {
        if (new_air_data == 1) {
            itoa(dht12.temp_int, stringA, 10);
            uart_puts("T ");
            uart_puts(stringA);
            uart_puts(".");
            oled_gotoxy(15, 2);
            oled_puts(stringA);
            oled_puts(".");
            itoa(dht12.temp_dec, stringA, 10);
            oled_gotoxy(18, 2);
            oled_puts(stringA);
            uart_puts(stringA);
            uart_puts(" °C\r\n");
            itoa(dht12.hum_int, stringA, 10);
            oled_gotoxy(15, 3);
            oled_puts(stringA);
            oled_puts(".");
            uart_puts("RH ");
            uart_puts(stringA);
            uart_puts(".");
            itoa(dht12.hum_dec, stringA, 10);
            uart_puts(stringA);
            uart_puts(" %\r\n");
            uart_puts("\r\n");
            oled_gotoxy(18, 3);
            oled_puts(stringA);
            oled_gotoxy(9, 4);
            
            if(moist_value>=890) {
                oled_puts("aktivni     ");
                GPIO_write_high(&PORTD, RELE);
                if(wr == 0){            
                    uint8_t data_w [4]= {RTC_now(RTC_ADR, 4), RTC_now(RTC_ADR, 5), RTC_now(RTC_ADR, 2), RTC_now(RTC_ADR, 1) };
                    eeprom_P_write(EEPROM_ADR, addr, data_w, 4);
                    wr = 1;
                    addr =+ 4; 
                }
            }
            else if(moist_value<=880){
                oled_puts("neaktivni");
                GPIO_write_low(&PORTD, RELE);
                wr = 0;
            }
            oled_display();
            
            
            // Do not print it again and wait for the new data
            new_air_data = 0;
            }
    }

    // Will never reach this
    return 0;
}

struct DHT_values_structure DHT_read(struct DHT_values_structure dht12){
    //struct DHT_values_structure dht;
    struct DHT_values_structure dht = dht12;
    twi_start();
    if (twi_write((SENSOR_ADR<<1) | TWI_WRITE) == 0) {
        // Set internal memory location
        twi_write(SENSOR_HUM_MEM);
        twi_stop();
        // Read data from internal memory
        twi_start();
        twi_write((SENSOR_ADR<<1) | TWI_READ);
        dht.hum_int = twi_read(TWI_ACK);
        dht.hum_dec = twi_read(TWI_ACK);
        dht.temp_int = twi_read(TWI_ACK);
        dht.temp_dec = twi_read(TWI_NACK);
        new_air_data = 1;
    }
    twi_stop();
    return dht;
}
/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/Counter1 overflow interrupt
 * Purpose:  Use single conversion mode and start conversion every 100 ms.
 **********************************************************************/
ISR(TIMER1_OVF_vect)
{
    //RTC_init(RTC_ADR,59,52,15,3,29,11,23);
    char string[4];  // String for converted numbers by itoa()
    // Read converted moist_value
    // Note that, register pair ADCH and ADCL can be read as a 16-bit moist_value ADC
    moist_value = get_moisture();
    // Convert "moist_value" to "string" and display it
    itoa(moist_value, string, 10);
    uart_puts("Vlhkost pudy: ");
    uart_puts(string);
    uart_putc('\n');

    uart_puts("Cas: ");
    //time_m[0] = RTC_now(0x68)[0]; 
    //uart_puts(itoa(sizeof(time), string, 10));
    itoa(RTC_now(0x68,2), string, 10);
    uart_puts(string);
    uart_putc(':');
    itoa(RTC_now(0x68,1), string, 10);
    uart_puts(string);
    uart_putc(':');
    itoa(RTC_now(0x68,0), string, 10);
    uart_puts(string);
    uart_puts("\r\n");
    /*
    twi_start();
    if (twi_write((SENSOR_ADR<<1) | TWI_WRITE) == 0) {
        // Set internal memory location
        twi_write(SENSOR_HUM_MEM);
        twi_stop();
        // Read data from internal memory
        twi_start();
        twi_write((SENSOR_ADR<<1) | TWI_READ);
        dht12.hum_int = twi_read(TWI_ACK);
        dht12.hum_dec = twi_read(TWI_ACK);
        dht12.temp_int = twi_read(TWI_ACK);
        dht12.temp_dec = twi_read(TWI_NACK);
    
        new_air_data = 1;
    }
    twi_stop();*/
    new_air_data = 1;
    dht12 = DHT_read(dht12);
    //itoa(eeprom_read(EEPROM_ADR, 0x0000, 255, data_r), string, 10);
    eeprom_read(EEPROM_ADR, 0x0004, 4, data_r);
    uart_putc('\n');
    uart_puts("read: ");
    //uart_puts(string);
    //uart_putc('\n');
    for(uint8_t i; i <4; i++){
      itoa(data_r[i], string, 10);
      uart_puts(string);
      uart_puts(", ");
    }
    //uart_putc('\n');
}


void display_text(void){
    oled_init(OLED_DISP_ON);
    oled_clrscr();
    oled_charMode(NORMALSIZE);
    oled_puts("Meteostanice");
    oled_gotoxy(0, 2);
    oled_puts("Teplota [°C]");
    oled_gotoxy(0, 3);
    oled_puts("Vlhkost RH[%]");
    oled_gotoxy(0, 4);
    oled_puts("Zalevani");
    oled_display();
}

void twi_test_devices(uint8_t address[], uint8_t n_devices){
    for(uint8_t i = 0; i < (n_devices - 1); i++){
        if (twi_test_address(address[i]) == 0)
            uart_puts("I2C sensor detected\r\n");
        else {
            uart_puts("[ERROR] I2C device not detected\r\n");
        //while (1);
    }
    }
}




