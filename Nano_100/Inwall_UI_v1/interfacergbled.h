#ifndef INTERFACERGBLED_H_INCLUDED
#define INTERFACERGBLED_H_INCLUDED

#include "Nano100Series.h"
#include "inwallconstants.h"


typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;

} rgbdriver;

extern const rgbdriver keusRGBDriver[RGB_DRIVER_LEDS];
extern const uint8_t inwallLEDID[RGB_DRIVER_LEDS];
extern void interfaceLEDInit(void);
extern void setInterfaceLED(uint8_t ledID, uint8_t red, uint8_t green, uint8_t blue);
extern void apply_update(uint8_t driverID);
extern void turnOffAllLed (void);
extern volatile uint8_t inwallSleepCounter, workingMode;
extern void inwallSplash(void);

#endif