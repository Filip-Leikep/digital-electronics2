#ifndef MOISTURE_SENS_H
# define MOISTURE_SENS_H
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

#endif