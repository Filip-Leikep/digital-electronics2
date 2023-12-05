
#include <uart.h>
#include <twi.h>
#include <eeprom_i2c.h>
#include "timer.h"
#include <avr/interrupt.h>
#include <stdlib.h>

#define EEPROM_ADR 0x57
volatile uint8_t r_w;


int main(void)
{
char string[4];

twi_init();
uart_init(UART_BAUD_SELECT(9600, F_CPU));



//itoa(eeprom_B_read(EEPROM_ADR, 0x0000), string, 10);
//uart_puts("read: ");
//uart_puts(string);
TIM1_OVF_1SEC
TIM1_OVF_ENABLE
    // Enables interrupts by setting the global interrupt mask
sei();

uart_puts("test\n");
itoa(twi_test_address(EEPROM_ADR), string, 10);
uart_puts(string);
/*itoa(eeprom_B_read(EEPROM_ADR, 0x0000), string, 10);
uart_puts("read: ");
uart_puts(string);*/

while (1)
    {

    }
}


ISR(TIMER1_OVF_vect)
{
  //uart_puts("intrpt");
  uart_puts("stav: ");
  uint8_t prnt;
  char string[4]; 
  uint8_t data_w[] = {1,2,30,4,100,6,7,18,9,10};
  uint8_t data_r[] = {0,0,0,0,0,0,0,0,0,0};
  /*
  if(r_w != 1){
    itoa(eeprom_P_write(EEPROM_ADR, 0x0120, data_w,10), string, 10);
    r_w = 1;
    uart_puts("write: ");
    uart_puts(string);
    uart_putc('\n');
  }else{
    prnt = eeprom_read(EEPROM_ADR, 0x0130, 10, data_r);
    itoa(prnt, string, 10);
    uart_puts("read: ");
    uart_puts(string);
    uart_putc('\n');
    r_w = 0;
    for(uint8_t i; i <10; i++){
      itoa(data_r[i], string, 10);
      uart_puts(string);
      uart_puts(", ");
    }
  }*/
  prnt = eeprom_read(EEPROM_ADR, 0x0120, 10, data_r);
    itoa(prnt, string, 10);
    uart_puts("read: ");
    uart_puts(string);
    uart_putc('\n');
    r_w = 0;
    for(uint8_t i; i <10; i++){
      itoa(data_r[i], string, 10);
      uart_puts(string);
      uart_puts(", ");
    }
}
