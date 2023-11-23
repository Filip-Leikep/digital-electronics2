#include <avr/io.h>
#include <avr/interrupt.h>
#include "moisture_sens.h"

volatile uint8_t new_sensor_data = 0;

ISR(ADC_vect)
{
    new_sensor_data = 1;
}

/*************************************************************************
 * Function: moisture_sens_init()
 * Purpose:  set and prepare ADC to convert
 * Input:    none
 * Returns:  none
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
 * Purpose:  start a ADC concersion and returtn converted value
 * Input:    none
 * Returns:  16 bit unsigned integer converted value 
 **************************************************************************/

unsigned short get_moisture(void){
    
    uint16_t value;
    // Start ADC conversion
    ADCSRA = ADCSRA | (1<<ADSC);

    if(new_sensor_data){
        // Read converted value
        // Note that, register pair ADCH and ADCL can be read as a 16-bit value ADC
        value = ADC;
        new_sensor_data = 0;
    }else{
        
    }
   return value;    
}