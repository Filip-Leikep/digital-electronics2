/***********************************************************************
 * 
 * Capacitive moisture sensor library for AVR-GCC.
 * 
 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO
 *
 *
 **********************************************************************/


/*=============INCLUDES==============*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include "moisture_sens.h"

/*===========GLOBAL VARIABLES=========*/
volatile uint8_t new_sensor_data = 0; /**< @brief Variabe for idicating that ad conversion has ended.*/

/*============INTERRUPT SERVICE RUTINES=================*/
/**********************************************************************
 * Function: End of ad conversion interrupt
 * Purpose:  Indicate that there is prepared new converted value
 **********************************************************************/
ISR(ADC_vect)
{
    new_sensor_data = 1; //inducete that ad conversion is done
}

/*************************************************************************
 * Function: moisture_sens_init()
 * Purpose:  set and prepare ADC to convert on pin PC0 (ADC0)
 * Input:    none
 * Returns:  none
 * Note:     Taken from: https://github.com/tomas-fryza/digital-electronics-2/tree/master/solutions/lab5-adc/src
 **************************************************************************/
void moisture_sens_init(void){

    // Configure Analog-to-Digital Convertion unit
    // Select ADC voltage reference to "AVcc with external capacitor at AREF pin"
    ADMUX = ADMUX | (1<<REFS0);
    // Select input channel ADC0 (voltage divider pin)
    ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2 | 1<<MUX1 | 1<<MUX0);
    // Enable ADC module
    ADCSRA = ADCSRA | (1<<ADEN);
    // Enable conversion complete interrupt
    ADCSRA = ADCSRA | (1<<ADIE);
    // Set clock prescaler to 128
    ADCSRA = ADCSRA | (1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0);
}

/*************************************************************************
 * Function: get_moisture()
 * Purpose:  start a ADC concersion on pin PC0 and returtn converted value
 * Input:    none
 * Returns:  16 bit unsigned integer converted value 
 **************************************************************************/

unsigned short get_moisture(void){
    uint16_t value;
    ADCSRA = ADCSRA | (1<<ADSC); //Start ADC conversion
    if(new_sensor_data){
        value = ADC; // Read converted value
        new_sensor_data = 0;    
    }else{
        
    }
   return value; //return converted value    
}