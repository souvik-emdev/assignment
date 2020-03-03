#ifndef INTERFACERGBLED_H_INCLUDED
#define INTERFACERGBLED_H_INCLUDED

#include "Nano100Series.h"

//Leftmost column
#define LED1 5
#define LED2 4
#define LED3 3
#define LED4 2
#define LED5 1
#define LED6 0
//2nd left column
#define LED7 6
#define LED8 10
#define LED9 7
#define LED10 8
#define LED11 9
#define LED12 11
//3rd left column
#define LED13 15
#define LED14 14
#define LED15 17
#define LED16 12
//Horizontals
#define LED17 22
#define LED18 21
#define LED19 20
#define LED20 23
//Rightmost
#define LED21 16
#define LED22 17
#define LED23 18
#define LED24 19

#define RED 0
#define GREEN 1
#define BLUE 2

#define MAX_NO_OF_DRIVERS 2

#define DRIVER_SLAVEID_1 0X3C
#define DRIVER_SLAVEID_2 0X3F

#define DRIVERID1 0
#define DRIVERID2 1



#define RGB_DRIVER_LEDS  24

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;

} rgbdriver;

extern const rgbdriver keusRGBDriver[RGB_DRIVER_LEDS];
extern void interfaceLEDInit(void);
extern void setInterfaceLED(uint8_t ledID, uint8_t colour, uint8_t brightness);

#endif