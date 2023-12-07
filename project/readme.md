# Smart plant watering system and data logger

### Team members
Graf Daniel, Leikep Filip, Lipenský Lukáš, Kubeš Václav

## Theoretical description and explanation
The goal of this project was to design an embedded system which would use Arduino UNO board together with other external modules to create an automatic plant watering system. The modules which were used are air humidity sensor, soil moisture sensor and a module with RTC and EEPROM. An LCD panel is attached to display time, air humidity and soil moisture value.


## Hardware description of demo application
Air humidity sensor DHT12  
Capacitive soil moisture sensor V1.2  
LCD SH1106  
RTC DS3231  
EPROM AT24C32  

![Device](images/device.jpg)

All components except the capacitive soil moisture sensor are connected through the I2C serial bus. The capacitive soil moisture sensor is connected to the pin `A0` to the internal ADC which converts the analog value to digital form which is then later used to determine the moisture of the soil. Watering process itself is in this project represented by an LED connected to the pin `PD2` which lights up when the watering process gets iniciated and turns off when the watering process ends but in real life application the LED would be replaced by a relay that would turn on/off a water pump. An RTC & EEPROM module with `DS3231 & AT24C32` IC`s is connected through I2C serial bus and keeps track of time and is also used for data logging. Temperature and relative humidity of air is measured by I2C sensor DHT12. An external OLED panel SH1106 is connected through I2C. It displays data such as air humidity (%), air temperature (°C), real time & date and the status of the watering process (active/inactive). Soil moisture isn't displayed because it is only an abstract number.

## Software description

Put flowchats of your algorithm(s) and direct links to source files in `src` or `lib` folders.
[lib](https://github.com/Filip-Leikep/digital-electronics2/tree/main/project/lib), [src]([https://www.google.com](https://github.com/Filip-Leikep/digital-electronics2/tree/main/project/src)

## Instructions

Write an instruction manual for your application, including photos and a link to a short app video.

## References

1. Write your text here.
2. ... 
