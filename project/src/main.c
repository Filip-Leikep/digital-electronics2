/***********************************************************************
 * Digital electronics 2: PROJECT
 * 
 * Description: AUTOMATED WATERING SYSTEM for watering plants. Uses capacitive 
 * moisture sensor for measuring humuduty of soil. Current date, time, watering
 * state and temerature and humiduty of surroundings is displayed on OLED dispay.
 * Every time it waters the plant time and date is logged into EEPROM memory.
 * 
 * Autors: Graf Daniel, Leikep Filip, Lipenský Lukáš, Kubeš Václav
 **********************************************************************/
/**
 * @file 
 *
 * @brief Main of automated plant watering project for Digital electronics 2 course.
 *
 * @author Vaclav Kubes
 * @author Filip Leikep
 * @author Daniel Graf
 * @author Lukas Lipnesky
 * @{
 */

/*======================================INCLUDES========================================*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <uart.h>           // UART library for AVR
#include <stdlib.h>         // C library. Needed for number conversions
#include <moisture_sens.h>  // Library for reading from moisture sensor
#include <oled.h>           // Library for using OLED display
#include <gpio.h>           // Libary for driving general purpous input/output ports
#include <rtc.h>            // Library for reading time form RTC module
#include <eeprom_i2c.h>     // Library for writing and reading from EEPROM chip on RTC module (via I2C)


/*=======================================DEFINES=======================================*/
#define SENSOR_ADR 0x5c /**<I2C address of DHT12 temeperature and humidity sensor*/
#define EEPROM_ADR 0x57 /**<I2C address of AT24C32 EEPROM*/
#define RTC_ADR 0x68    /**<I2C address of RTC DS3231*/
#define N_I2C_DEVICES 3 /**<Number of devices connected to I2C - OLED not included*/

#define SENSOR_HUM_MEM 0 
#define SENSOR_TEMP_MEM 2
#define SENSOR_CHECKSUM 4
#define DRY 890 /**<Value which corresponds with dry soil*/
#define WET 880 /**<Value which corresponds with wet soil*/

#define RELE PD2 /**<Relay is connected to pin PD2 of ATmegs328P*/
#define BUTTON_DATA PD3 /**<Button is connected to pin PD3 of ATmegs328P*/
#define LED PB5 /**<Indication led is connected to pin PB5 of ATmegs328P*/


/*===================================DECLARING GLOBAL VARIABLES============================*/
volatile uint8_t new_data = 0;  /**<Signalize that 1 second has passed*/
uint16_t eep_addr = 0;          /**<Address of memory cell in eeprom*/
uint16_t moist_value;           /**<Value from moisture sensor*/
uint8_t eep_log_done = 0;       /**<Signalize if time of watering was logged to eeprom*/
uint8_t data_r;                 /**<Byte of data read from eeprom*/
uint8_t time_from_rtc [6] = {0,0,0,0,0,0};      /**<Store data loaded form RTC module*/

struct DHT_values_structure {           /**<Store temperature and humidity fromm DHT12*/
   uint8_t hum_int;     /**<Integer part of humidity*/
   uint8_t hum_dec;     /**<Decimal part of humidity*/
   uint8_t temp_int;    /**<Integer part of temperature*/
   uint8_t temp_dec;    /**<decimal part of temperature*/
   uint8_t checksum;    /**<Control check sum*/
};

struct DHT_values_structure dht12;

/*==================================DECLARING FUNCTIONS====================================*/
struct DHT_values_structure DHT_read(struct DHT_values_structure dht12);        /**<Function for reading temp and humidity form DHT12*/
void display_text(void);            /**<Display non static text on OLED*/
void update_vals_oled();            /**<Write new data (temp, humidity, watering state, time, date) on OLED*/
void update_vals_uart();            /**<Write new data to serial monitor - do not use: behavior of main is not stable*/
void data_read(void);           /**<Read logged times  of watering and print them to serial monitor*/
void watering(void);            /**<Evaluate moisture of soil, water the plant and log the time*/


/*==================================FUNCTIONS DEFINITON======================================*/

/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Ensure all the functions of wattering system
 * Returns:  none
 **********************************************************************/
int main(void)
{
    uint8_t check = 1;
    uint8_t btnPress;

    twi_init();         //initialization of i2c
    RTC_init(RTC_ADR,59,55,15,3,29,11,23);      //set the currrent time to RTC
    
    uart_init(UART_BAUD_SELECT(115200, F_CPU));     //initialization of serial comunication with PC
    uart_putc('\n');
    uart_putc('\n');
    uart_puts("START");
    uart_putc('\n');
    
    moisture_sens_init();       //prepare moisture sensor and ADC to read new values

    TIM1_OVF_1SEC       //set timer 1 to count 1 second
    TIM1_OVF_ENABLE
    
    sei();      // Enables interrupts by setting the global interrupt mask

    GPIO_mode_output(&DDRD, RELE);      //set GPIO for driving relay and signalizing LED
    GPIO_mode_output(&DDRB, LED);
    GPIO_mode_input_pullup(&DDRD, BUTTON_DATA);     //set GPIO to read the button state

    display_text();     //display static text on OLED

    
    while (1)       // Infinite loop
    {

        if (new_data == 1) {        //if 1 second has passed then:
            moist_value = get_moisture();       //read the moisture sensor
            dht12 = DHT_read(dht12);        //read current time and date
            time_from_rtc[0] = RTC_now(RTC_ADR,0);
            time_from_rtc[1] = RTC_now(RTC_ADR,1);
            time_from_rtc[2] = RTC_now(RTC_ADR,2);
            time_from_rtc[3] = RTC_now(RTC_ADR,4);
            time_from_rtc[4] = RTC_now(RTC_ADR,5);
            time_from_rtc[5] = RTC_now(RTC_ADR,6);
            
            //update_vals_uart();
            update_vals_oled();     //display newly obtainet data on OLED
            watering();         //evaluate the moisutere of soil and if its needed then water the plant
            oled_display();     //refresh OLEDs
            
            new_data = 0;       // Do not print it again and wait for 1 sec
            }

        btnPress = GPIO_read(&PIND, BUTTON_DATA);

        if(btnPress == 0) {     //if button is pressed then print the logged times of watering to serial monitor
            if (check==1) {
                data_read();
                check = 0;
            }
        }else {
            check = 1;
        }
    }
    return 0;       // Will never reach this
}


/*============================INTERRUPT SERVICE ROUTINES=======================================*/

/**********************************************************************
 * Function: Timer/Counter1 overflow interrupt
 * Purpose:  Time counting for 1 sec
 **********************************************************************/
ISR(TIMER1_OVF_vect)
{
    new_data = 1;       //signalize tahat 1 sec has passed
}

/*====================================FUNCTIONS DEFINITION========================================*/

/**********************************************************************
 * Function: display_text()
 * Purpose:  Display static text on OLED
 * Return: none
 **********************************************************************/
void display_text(void){
    oled_init(OLED_DISP_ON);
    oled_clrscr();
    oled_charMode(NORMALSIZE);
    oled_puts("Zavlazovani");
    oled_gotoxy(0, 1);
    oled_puts("Teplota [°C]");
    oled_gotoxy(0, 2);
    oled_puts("Vlhkost RH[%]");
    oled_gotoxy(0, 3);
    oled_puts("Zalevani");
    oled_gotoxy(0, 4);
    oled_puts("Cas");
    oled_gotoxy(0, 5);
    oled_puts("Datum");
    oled_display();
}

/**********************************************************************
 * Function: update_vals_oled()
 * Purpose:  Display new values of date, time, temp, humidity and watering state text on OLED
 * Return: none
 **********************************************************************/
void update_vals_oled(void){
    char string[2];     //String for converting numbers by itoa()
    
    itoa(dht12.temp_int, string, 10);
    oled_gotoxy(15, 1);
    oled_puts(string);
    oled_puts(".");

    itoa(dht12.temp_dec, string, 10);
    oled_gotoxy(18, 1);
    oled_puts(string);
    oled_gotoxy(19, 1);
    oled_puts(" ");

    itoa(dht12.hum_int, string, 10);
    oled_gotoxy(15, 2);
    oled_puts(string);
    oled_puts(".");

    itoa(dht12.hum_dec, string, 10);
    oled_gotoxy(18, 2);
    oled_puts(string);
    oled_gotoxy(19, 2);
    oled_puts(" ");
    

    itoa(time_from_rtc[2], string, 10);
    oled_gotoxy(6, 4);
    if (time_from_rtc[2]<10){
        oled_puts("0");
    }
    oled_puts(string);
    oled_putc(':');
    itoa(time_from_rtc[1], string, 10);
    if (time_from_rtc[1]<10){
        oled_puts("0");
    }
    oled_puts(string);
    oled_putc(':');
    itoa(time_from_rtc[0], string, 10);
    if (time_from_rtc[0]<10){
        oled_puts("0");
    }
    oled_puts(string);
    oled_gotoxy(14, 4);
    oled_puts(" ");

    itoa(time_from_rtc[3], string, 10);
    oled_gotoxy(6, 5);
    if (time_from_rtc[3]<10){
        oled_puts("0");
    }
    oled_puts(string);
    oled_puts(". ");
    itoa(time_from_rtc[4], string, 10);
    if (time_from_rtc[4]<10){
        oled_puts("0");
    }
    oled_puts(string);
    oled_puts(". ");
    oled_puts("20");
    itoa(time_from_rtc[5], string, 10);
    oled_puts(string);
}

/**********************************************************************
 * Function: update_vals_uart()
 * Purpose:  Print new values of date, time, temp, humidity and moisture to serial monitor
 * Return: none
 **********************************************************************/
void update_vals_uart(void){
    char string[2];     //String for converting numbers by itoa()
    uart_putc('\n');
    itoa(dht12.temp_int, string, 10);
    uart_puts("T ");
    uart_puts(string);
    uart_puts(".");
    
    itoa(dht12.temp_dec, string, 10);
    uart_puts(string);
    uart_puts(" °C\r\n");

    itoa(dht12.hum_int, string, 10);
    uart_puts("RH ");
    uart_puts(string);
    uart_puts(".");

    itoa(dht12.hum_dec, string, 10);
    uart_puts(string);
    uart_puts(" %\r\n");
    
    itoa(moist_value, string, 10);
    uart_puts("Vlhkost pudy: ");
    uart_puts(string);
    uart_putc('\n');

    uart_puts("Cas: ");
    itoa(time_from_rtc[2], string, 10);
    uart_puts(string);
    uart_putc(':');
    itoa(time_from_rtc[1], string, 10);
    uart_puts(string);
    uart_putc(':');
    itoa(time_from_rtc[0], string, 10);
    uart_puts(string);
    uart_puts("\r\n");
    uart_puts("\r\n");
}


/**********************************************************************
 * Function: watering()
 * Purpose:  Evaluate if watering is needed, if so water the plant and log the time of watering
 * Return: none
 **********************************************************************/
void watering(void){
    if(moist_value >= DRY) {
        oled_gotoxy(9, 3);
        oled_puts("aktivni     ");      //inform  abyout watering on OLED
        GPIO_write_high(&PORTD, RELE);
        if(eep_log_done == 0){      //log time of watering just once a time
            GPIO_write_high(&PORTB, LED);            
            uint8_t data_w [4]= {time_from_rtc[3], time_from_rtc[4], time_from_rtc[2], time_from_rtc[1]};
            eeprom_P_write(EEPROM_ADR, eep_addr, data_w, 4);
            eep_log_done = 1;
            eep_addr += 4;      //prepare memory address for new writing
            }
        }else if(moist_value <= WET){
            oled_gotoxy(9, 3);
            oled_puts("neaktivni");
            GPIO_write_low(&PORTD, RELE);
            eep_log_done = 0;
            GPIO_write_low(&PORTB, LED); 
        }
    
}

/**********************************************************************
 * Function: DHT_read()
 * Purpose:  Obtain new values of temperature and humiditay form DHT12
 * Return: structure of integer part and decimal part of temperature and humidity
 **********************************************************************/
struct DHT_values_structure DHT_read(struct DHT_values_structure dht12){
    struct DHT_values_structure dht = dht12;
    twi_start();
    if (twi_write((SENSOR_ADR<<1) | TWI_WRITE) == 0) {
        twi_write(SENSOR_HUM_MEM);
        twi_stop();
        twi_start();
        twi_write((SENSOR_ADR<<1) | TWI_READ);
        dht.hum_int = twi_read(TWI_ACK);
        dht.hum_dec = twi_read(TWI_ACK);
        dht.temp_int = twi_read(TWI_ACK);
        dht.temp_dec = twi_read(TWI_NACK);
    }
    twi_stop();
    return dht;
}

/**********************************************************************
 * Function: data_read()
 * Purpose:  Read data saved in EEPROM and write them to serial monitor
 * Return: none
 **********************************************************************/
void data_read(void){
    char string [2];
    uart_putc('\n');
    uart_puts("Výpis časů posledních zalití: ");
    uart_putc('\n'); 
    for (uint16_t i = 0; i < 4096 ; i++)
    {
        data_r = eeprom_B_read(EEPROM_ADR, i);
        if(data_r != 255){
        //eep_addr =+ 1;
        itoa(data_r, string, 10);
        if(((i+1) % 4) != 0){
            uart_puts(string);
            uart_puts(", ");
        }else{
            uart_puts(string);
            uart_putc('\n');
        }
        }else{
            return;
        }
    }
}
/** @} */