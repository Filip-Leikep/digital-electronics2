#ifndef RTC_H
# define RTC_H

/**
 * @file 
 * @defgroup RTC library
 * @code #include <rtc.h> @endcode
 *
 * @brief RTC library for AVR-GCC.
 *
 * The library contains functions for using RTC module.
 * @{
 */


#include <avr/io.h>


/**
 * @brief  Set RTC time values. Tme 
 * @param  I2C address of RTC module
 * @param  current seconds (0 - 59)
 * @param  current minutes (0 - 59)
 * @param  current hour (0 - 23)
 * @param  current day (1 - Sunday...)
 * @param  current date (1 - 31)
 * @param  current month (1 - 12)
 * @param  current year (0 - 99)
 * @return none
 */
void RTC_init(uint8_t rtc_i2c_address, uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t day, uint8_t date, uint8_t month, uint8_t year);


/**
 * @brief  Get information about current time, date and year.
 * @param  I2C address of RTC module
 * @return arry of 7 elements: [0] seconds, [1] minutes, [2] hours, [3] day, [4] date, [5] month, [6] year
 */
uint8_t *RTC_now(uint8_t rtc_i2c_address);



#endif