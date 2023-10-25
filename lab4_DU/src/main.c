/***********************************************************************
 * 
 * Stopwatch by Timer/Counter2 on the Liquid Crystal Display (LCD).
 *
 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO
 *
 * Copyright (c) 2017 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 * Components list:
 *   16x2 character LCD with parallel interface
 *     VSS  - GND (Power supply ground)
 *     VDD  - +5V (Positive power supply)
 *     Vo   - (Contrast)
 *     RS   - PB0 (Register Select: High for Data transfer, Low for Instruction transfer)
 *     RW   - GND (Read/Write signal: High for Read mode, Low for Write mode)
 *     E    - PB1 (Read/Write Enable: High for Read, falling edge writes data to LCD)
 *     D3:0 - NC (Data bits 3..0, Not Connected)
 *     D4   - PD4 (Data bit 4)
 *     D5   - PD5 (Data bit 5)
 *     D6   - PD6 (Data bit 6)
 *     D7   - PD7 (Data bit 7)
 *     A+K  - Back-light enabled/disabled by PB2
 * 
 **********************************************************************/


/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <gpio.h>           // GPIO library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <lcd.h>            // Peter Fleury's LCD library
#include <stdlib.h>         // C library. Needed for number conversions


/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Update stopwatch value on LCD screen when 8-bit 
 *           Timer/Counter2 overflows.
 * Returns:  none
 **********************************************************************/
static uint8_t minutes = 0;
static uint8_t seconds = 0;
static uint16_t secsq;
char textRUN[22] = "   Atmega qwertzuiop\n";
char zobraz[22];
int i;

int main(void)
{
    // Initialize display
    lcd_init(LCD_DISP_ON_CURSOR);
    TIM2_OVF_16MS
    TIM2_OVF_ENABLE

    TIM1_OVF_262MS
    TIM1_OVF_ENABLE

    /*
    uint8_t zkouska = 0;
    char Sminutes = itoa(minutes, Sminutes, 10);
    char Wminutes = ("0%s", Sminutes);
    lcd_gotoxy(1, 0);
    lcd_puts(Wminutes);
    lcd_putc(':');
    */
    lcd_gotoxy(2,0);
    lcd_putc(':');
    lcd_gotoxy(5,0);
    lcd_putc('.');
    
    
    // Configuration of 8-bit Timer/Counter2 for Stopwatch update
    // Set the overflow prescaler to 16 ms and enable interrupt


    // Enables interrupts by setting the global interrupt mask
    sei();

    // Infinite loop
    while (1)
    {
        /* Empty loop. All subsequent operations are performed exclusively 
         * inside interrupt service routines, ISRs */
    }

    // Will never reach this
    return 0;
}


/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/Counter2 overflow interrupt
 * Purpose:  Update the stopwatch on LCD screen every sixth overflow,
 *           ie approximately every 100 ms (6 x 16 ms = 100 ms).
 **********************************************************************/
ISR(TIMER2_OVF_vect)
{
    static uint8_t no_of_overflows = 0;
    static uint8_t tenths = 0;  // Tenths of a second
    char tenthsS[2];             // String for converted numbers by itoa()
    char secondsS[2];
    char minutesS[2];
    char secsqS[4];

    no_of_overflows++;
    if (no_of_overflows >= 6)
    {
        // Do this every 6 x 16 ms = 100 ms
        no_of_overflows = 0;

        // Count tenth of seconds 0, 1, ..., 9, 0, 1, ...
        tenths++;
        if(tenths==10) {
          seconds++;
          tenths = 0;
        }
        if(seconds==60) {
          minutes++;
          seconds = 0;
          lcd_gotoxy(9, 0);
          lcd_puts("    ");
        }

        itoa(minutes, minutesS, 10);  // Convert decimal value to string
        if(minutes<10) {
          lcd_gotoxy(0, 0);
          lcd_puts("0");
          lcd_puts(minutesS);
        }
        else {
          lcd_gotoxy(0, 0);
          lcd_puts(minutesS);
        }
        
        itoa(seconds, secondsS, 10);  // Convert decimal value to string
        if(seconds<10) {
          lcd_gotoxy(3, 0);
          lcd_puts("0");
          lcd_puts(secondsS);
        }
        else{
          lcd_gotoxy(3, 0);
          lcd_puts(secondsS);
        }
        
        itoa(tenths, tenthsS, 10);  // Convert decimal value to string
        // Display "00:00.tenths"
        lcd_gotoxy(6, 0);
        lcd_puts(tenthsS);

        secsq = seconds*seconds;
        itoa(secsq, secsqS, 10);  
        lcd_gotoxy(9, 0);
        lcd_puts(secsqS);

    }
    // Else do nothing and exit the ISR
}

ISR(TIMER1_OVF_vect) {
  static uint8_t tim1ovf = 0;
  static uint8_t pozice = 0;
  static uint8_t temp = 0;

  tim1ovf++;
  if(tim1ovf >= 2) {
    tim1ovf = 0;

    while(textRUN[pozice] != '\0') {
            zobraz[temp] = textRUN[pozice];
            temp++;
            pozice++;
        }

    zobraz[temp] = '\0';
    pozice++;
    if(pozice == sizeof(textRUN)) {
      pozice = 0;
    }
    lcd_gotoxy(9, 1);
    lcd_puts(zobraz);
  }
}