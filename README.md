# Clock

Clock with stopwatch
 
### Parts
1. Arduino Nano (or another, which one do you have or which do you like more).
2. DS1302 module
3. LCD 1602 Dislpay with I2C adapter
4. 3 buttons
5. 1 led
6. 220R resistor

### Libs
1. [`<iarduino_RTC.h>`](https://iarduino.ru/file/235.html) - 1.3.4v
2. [`<LiquidCrystal_I2C.h>`](https://www.arduinolibraries.info/libraries/liquid-crystal-i2-c) - 1.1.2v

## How to connect
### DS1302 module

| DS1302 | Arduino Nano |
|:------:|:------------:|
`RST`  | `D6`
`DAT`  | `D7`
`CLK`  | `D8`
`GND`  | `GND`
`VCC`  | `5v`

### 1602 LCD display
the display must be connected to the `I2C` interface on the arduino through an adapter.

| LCD 1602 | Arduino Nano |
|:-------:|:-------------:|
`SDA`    | `A4`
`SCL`    | `A5`
`GND`    | `GND`
`VSS`    | `5v`


### Buttons 
All buttons must be connected to `GND` and digital pin on arduino
| Button | Arduino Nano |
|:------:|:-------------:|
Start/Stop | `D4`
Pause | `D3`
Show  | `D5`

 ### LED
 The anode `+` must be connected to `D13` on arduino 
 
 The cathode `-` must be connected to `GND` throught a resistor (220R)




