
#include <uart.h>
#include <twi.h>
#include <eeprom_i2c.h>
#include "timer.h"
#include <avr/interrupt.h>
#include <stdlib.h>

#define EEPROM_ADR 0x57
volatile uint8_t r_w;
uint8_t data_w[4] = {1,4,8,56};// = {0,0,0,0,0,0,0,0,0,0};
uint8_t data_r = 0;
uint16_t adr = 0;

int main(void)
{
char string[8];

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

uart_putc('\n');
    uart_puts("Výpis časů posledních zalití: ");
    uart_putc('\n');
    //data_r = eeprom_B_read(EEPROM_ADR, eep_addr); 
    //itoa(data_r, string, 10);
    //uart_puts(string);
    //uart_puts(", ");   
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
          break;
        }
    }

while (1)
    {
      if(r_w){
        itoa(adr, string, 10);
        uart_putc('\n');
        uart_puts("adresa: ");
        uart_puts(string);
        uart_putc('\n');
        uint8_t stav =  eeprom_P_write(EEPROM_ADR, adr, data_w, 4);
        itoa(stav, string, 10);
        uart_putc('\n');
        uart_puts("stav: ");
        uart_puts(string);
        uart_putc('\n');
        adr += 4;
        r_w = 0;
      }
    }
}


ISR(TIMER1_OVF_vect)
{
  //uart_puts("intrpt");
  //uart_puts("stav: ");
  //uint8_t prnt;
  //char string[4]; 
  //uint8_t data_w[] = {1,2,30,4,100,6,7,18,9,10};
  //uint8_t data_r[] = {0,0,0,0,0,0,0,0,0,0};
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
  /*
  prnt = eeprom_read(EEPROM_ADR, 0x0000, 10, data_r);
    itoa(prnt, string, 10);
    uart_puts("read: ");
    uart_puts(string);
    uart_putc('\n');
    r_w = 0;
    for(uint8_t i; i <10; i++){
      itoa(data_r[i], string, 10);
      uart_puts(string);
      uart_puts(", ");
    }*/
    /*
  for(uint8_t i; i <32; i++){
    prnt = eeprom_B_read(EEPROM_ADR, 0x0000+i);
    itoa(prnt, string, 10);
    uart_puts("read: ");
    uart_puts(string);
    uart_putc('\n');
  }*/
  /*
  prnt = eeprom_B_read(EEPROM_ADR, 0x0032);
    itoa(prnt, string, 10);
    uart_puts("read: ");
    uart_puts(string);*/
    r_w = 1;
}
