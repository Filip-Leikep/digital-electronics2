/***********************************************************************
 * 
 * Knight Rider using interruption 
 * 
 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO
 *
 * Copyright (c) 2018 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/


/* Defines -----------------------------------------------------------*/
#define LED_1 PB1
#define LED_2 PB2
#define LED_3 PB3
#define LED_4 PB4
#define LED_5 PB5
#define LED_IND PB6

#define TL PD7


/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <gpio.h>           // GPIO library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC


/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Toggle two LEDs using the internal 8- and 16-bit 
 *           Timer/Counter.
 * Returns:  none
 **********************************************************************/
uint8_t running = 0;
uint8_t nextLED = 0;
uint8_t counterLED = 1;
uint8_t check = 1;

int main(void)
{
    uint8_t btnPress;
    GPIO_mode_output(&DDRB, LED_1);
    GPIO_mode_output(&DDRB, LED_2);
    GPIO_mode_output(&DDRB, LED_3);
    GPIO_mode_output(&DDRB, LED_4);
    GPIO_mode_output(&DDRB, LED_5);
    GPIO_mode_output(&DDRB, LED_IND);
    GPIO_mode_input_nopull(&DDRD, TL);
    
    // Configuration of 16-bit Timer/Counter1 for LED blinking
    // Set the overflow prescaler to 262 ms and enable interrupt
    TIM1_OVF_262MS
    TIM1_OVF_ENABLE

    // Enables interrupts by setting the global interrupt mask
    sei();

    // Infinite loop
    while (1)
    {
        btnPress = GPIO_read(&PIND, TL);

      if(btnPress == 1) {
        if ((running==0) & (check==1)) {
          running = 1;
          GPIO_write_high(&PORTB, LED_IND);
          check = 0;
        }
        else if((running==1) & (check==1)) {
          running = 0;
          GPIO_write_low(&PORTB, LED_IND);
          check = 0;
        }
      }
      else {
        check = 1;
      }
    }

    // Will never reach this
    return 0;
}


/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/Counter1 overflow interrupt
 * Purpose:  Toggle on-board LED.
 **********************************************************************/
ISR(TIMER1_OVF_vect)
{
    if(running == 1) {
        GPIO_write_low(&PORTB, (LED_1 + nextLED - 1));
        nextLED += counterLED;
        if(nextLED==5) {
            counterLED = -1;
        } 
        else if(nextLED==1) {
            counterLED = 1;
        }
        GPIO_write_high(&PORTB, (LED_1 + nextLED - 1));
    }
    else {
        nextLED = 0;
        GPIO_write_low(&PORTB, LED_1);
        GPIO_write_low(&PORTB, LED_2);
        GPIO_write_low(&PORTB, LED_3);
        GPIO_write_low(&PORTB, LED_4);
        GPIO_write_low(&PORTB, LED_5);
    }
}