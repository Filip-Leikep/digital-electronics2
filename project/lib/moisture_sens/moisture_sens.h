#ifndef MOISTURE_SENS_H
# define MOISTURE_SENS_H

/***********************************************************************
 * 
 * Capacitive moisture sensor library for AVR-GCC.
 * 
 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO
 *
 *
 **********************************************************************/


/**
 * @file 
 *
 * @brief Capacitive moisture sensor library for AVR-GCC.
 *
 * This library defines functions for using capacitive moisture sensor (i.e.: using ADC of ATmega328P)
 *
 * @author Vaclav Kubes
 * @note Great portion of code is taken from doc. Tomas Fryza github: https://github.com/tomas-fryza/digital-electronics-2/tree/master/solutions/lab5-adc/src
 * @{
 */

/**
 *  @brief   Sets the ADC peripheral and prepare for ADC conversion on PC0 (A0)
 *
 *  @param   none 
 *  @return  none
 */
extern void moisture_sens_init(void);

/**
 *  @brief   Starts the ADC conversion on pin PC0 (A0) and returns converted value
 *
 *  @param   none 
 *  @return  16 bit unsigned integer of converted value 
 */
extern unsigned short get_moisture(void);

/** @} */
#endif