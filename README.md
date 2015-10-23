# rc-leonardo-joy
Adaptor for RC simulators based on Arduino Leonardo. Accepts direct PPM input from radio

This project based on two another projects:

1. [UnoJoy](https://github.com/AlanChatham/UnoJoy)

2. [Add USB Game Controller to Arduino Leonardo/Micro](http://www.instructables.com/id/Add-USB-Game-Controller-to-Arduino-LeonardoMicro/step9/Joystick-Library/#step1) 

I've combined them into one. So you do not need to flash Atmega32U2 on Arduino Mega board. You need only Arduino Leonardo/Micro board with Atmega32U4. 

You should replace two files in Arduino folder **hid.cpp** and **usbapi.h**
They slightly differs from MatthewH's ones.
I use Arduino 1.6.5 r5. Choose board Leonardo and simply flash it with sketch

Connect Arduino Ground and D0(RX) pins to PPM output of your receiver. 
In Windows, find Arduino Leonardo in Devices and Printers. Then right click on it and choose Game device Parameters. Calibrate joystick there using your RC transmitter connected

Initially project was designed to be user with Turnigy 9XR and FPV FreeRider simulator. I don't know how it will work with other transmitters and simulators.
