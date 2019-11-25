#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "Mini58Series.h"
#include "irBluster.h"
//#define ir_pin P32

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)

extern uint8_t ir_out2[];

void setAsGpio(void)
{
    SYS->P2_MFP &= ~SYS_MFP_P22_Msk;
    SYS->P2_MFP |= SYS_MFP_P22_GPIO;
    GPIO_SetMode(P2, BIT2, GPIO_MODE_OUTPUT);
    P22 = 0;
}

void setAsPwm(void)
{
    SYS->P2_MFP = SYS_MFP_P22_PWM0_CH0;
}

void tx1()
{
    //ir_pin = 1;
    setAsPwm();
    TIMER_Delay(TIMER0, 394);
    //ir_pin = 0;
    setAsGpio();
    TIMER_Delay(TIMER0, 1180);
}

void tx0()
{
    //ir_pin = 1;
    setAsPwm();
    TIMER_Delay(TIMER0, 406);
    //ir_pin = 0;
    setAsGpio();
    TIMER_Delay(TIMER0, 382);
}

void startbit()
{
    //ir_pin = 1;
    setAsPwm();
    TIMER_Delay(TIMER0, 3168);
    //ir_pin = 0;
    setAsGpio();
    TIMER_Delay(TIMER0, 568);
}

void stopbit()
{
    //ir_pin = 1;
    setAsPwm();
    TIMER_Delay(TIMER0, 406);
    //ir_pin = 0;
    setAsGpio();
}

void transmit()
{
    startbit();
    bool bitvalue = 0;
    for (int i = 1; i < 12; i++)
    {
        for (int j = 7; j >= 0; j--)
        {
            bitvalue = bitRead(ir_out2[i], j);
            //printf("%d", bitvalue);
            if (bitvalue)
            {
                tx1();
            }
            else
            {
                tx0();
            }
        }
        //printf(" ");
    }
    stopbit();
}
