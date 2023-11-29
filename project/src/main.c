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

#define SENSOR_ADR 0x5c
#define SENSOR_HUM_MEM 0
#define SENSOR_TEMP_MEM 2
#define SENSOR_CHECKSUM 4

#define RTC_ADR 0x68

#define RELE PD2 

volatile uint8_t new_air_data = 0;
volatile uint8_t new_time_data = 0;
volatile uint8_t water = 0;
uint16_t moist_value;

struct DHT_values_structure {
   uint8_t hum_int;
   uint8_t hum_dec;
   uint8_t temp_int;
   uint8_t temp_dec;
   uint8_t checksum;
} dht12;


/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Use Timer/Counter1 and start ADC conversion every 100 ms.
 *           When AD conversion ends, send converted value to LCD screen.
 * Returns:  none
 **********************************************************************/
int main(void)
{
    // Initialize display
    uart_init(UART_BAUD_SELECT(115200, F_CPU));
    char stringA[2];  // String for converting numbers by itoa()
    uart_puts("test");
    // Configure Analog-to-Digital Convertion unit
    // Select ADC voltage reference to "AVcc with external capacitor at AREF pin"
    moisture_sens_init(); //inicializace půdní vlhkosti
    twi_init(); //inicializace i2c
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
    uint8_t addresses = {SENSOR_ADR, RTC_ADR};
    twi_test_devices(addresses);


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
            
            if(moist_value>=800) {
                oled_puts("aktivni     ");
                GPIO_write_high(&PORTD, RELE);
            }
            else if(moist_value<=680){
                oled_puts("neaktivni");
                GPIO_write_low(&PORTD, RELE);
            }
            oled_display();
            
            // Do not print it again and wait for the new data
            new_air_data = 0;
            }
    }

    // Will never reach this
    return 0;
}


/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/Counter1 overflow interrupt
 * Purpose:  Use single conversion mode and start conversion every 100 ms.
 **********************************************************************/
ISR(TIMER1_OVF_vect)
{
    char string[4];  // String for converted numbers by itoa()
    // Read converted moist_value
    // Note that, register pair ADCH and ADCL can be read as a 16-bit moist_value ADC
    moist_value = get_moisture();
    // Convert "moist_value" to "string" and display it
    itoa(moist_value, string, 10);
    uart_puts(string);
    uart_putc('\n');
    
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
    twi_stop();
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

void twi_test_devices(uint8_t address[]){
    for(uint8_t i; i++; i == sizeof(address)/8 - 1){
        if (twi_test_address(address[i]) == 0)
            uart_puts("I2C sensor detected\r\n");
        else {
            uart_puts("[ERROR] I2C device not detected\r\n");
        while (1);
    }
    }
}
