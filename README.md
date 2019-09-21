These programs are written completely in C with the Arduino IDE.

Two development boards were used for these tasks: Arduino UNO and ESP8266.

Brief of tasks:

#Task1:
https://github.com/souvik-emdev/assignment/tree/master/Switch_Push-Button_Led_Logic

This task was done on ESP8266. Two slide switches, two push buttons and four LEDs were connected to the GPIOs. The aim was to blink the four LEDs at different rate combinations based on the slide switch position and push button press action.
More details may be found on the task readme.

#Task2:
https://github.com/souvik-emdev/assignment/tree/master/Communication_Using_GPIO

The aim of this task was to transmit data from one microcontroller to another without using any communication peripheral and do some actions on the receiver side based in the transmitted data.
One Arduino UNO was acting as the transmitter and one ESP8266 was acting as receiver.
Synchronous serial communication was achieved using three GPIO pins connected between the transreceiver.