/**************************************************************************/ /**
 * @file     main.c
 * @version  V2.00
 * $Revision: 1 $
 * $Date: 14/09/19 3:18p $
 * @brief    An I2C master/slave demo by connecting I2C0 and I2C1 interface.
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "Nano100Series.h"
#include "interfacergbled.h"
#include "inwallconstants.h"
#include "inwallI2C.h"
#include "inwalluart.h"
#include "inwallactions.h"
#include "inwallbutton.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

uint8_t tester = 0;

void inwallLoop(void)
{
    while (1)
    {

        if (keusAppEvents)
        {
            tester++;
        }
        else
            continue;

        if (keusAppEvents & INWALL_SPLASH_EVT)
        {
            inwallSplash();
            keusAppEvents ^= INWALL_SPLASH_EVT;
        }
        else if (keusAppEvents & INWALL_WATCHDOG_EVT)
        {
            PB8 ^= 1;

            keusAppEvents ^= INWALL_WATCHDOG_EVT;
        }
        else if (keusAppEvents & INWALL_BUTTON1)
        {
            
            // if (!workingMode)
            // inwallWakeUp();
            switchPressAction(SWITCH_ID1);

            keusAppEvents ^= INWALL_BUTTON1;
        }
        else if (keusAppEvents & INWALL_BUTTON2)
        {
            switchPressAction(SWITCH_ID2);
            keusAppEvents ^= INWALL_BUTTON2;
        }
        else if (keusAppEvents & INWALL_BUTTON3)
        {
            switchPressAction(SWITCH_ID3);
            keusAppEvents ^= INWALL_BUTTON3;
        }
        else if (keusAppEvents & INWALL_BUTTON4)
        {
            switchPressAction(SWITCH_ID4);
            keusAppEvents ^= INWALL_BUTTON4;
        }
        else if (keusAppEvents & INWALL_BUTTON5)
        {
            switchPressAction(SWITCH_ID5);
            keusAppEvents ^= INWALL_BUTTON5;
        }
        else if (keusAppEvents & INWALL_BUTTON6)
        {
            switchPressAction(SWITCH_ID6);
            keusAppEvents ^= INWALL_BUTTON6;
        }
        else if (keusAppEvents & INWALL_BUTTON7)
        {
            switchPressAction(SWITCH_ID7);
            keusAppEvents ^= INWALL_BUTTON7;
        }
        else if (keusAppEvents & INWALL_BUTTON8)
        {
            switchPressAction(SWITCH_ID8);
            keusAppEvents ^= INWALL_BUTTON8;
        }
        else if (keusAppEvents & INWALL_BUTTON9)
        {
            switchPressAction(SWITCH_ID9);
            keusAppEvents ^= INWALL_BUTTON9;
        }
        else if (keusAppEvents & INWALL_BUTTON10)
        {
            switchPressAction(SWITCH_ID10);
            keusAppEvents ^= INWALL_BUTTON10;
        }

        else if (keusAppEvents & INWALL_BUTTON11)
        {
            switchPressAction(SWITCH_ID11);
            keusAppEvents ^= INWALL_BUTTON11;
        }
        else if (keusAppEvents & INWALL_BUTTON12)
        {
            switchPressAction(SWITCH_ID12);
            keusAppEvents ^= INWALL_BUTTON12;
        }
        else if (keusAppEvents & INWALL_BUTTON13)
        {
            switchPressAction(SWITCH_ID13);
            keusAppEvents ^= INWALL_BUTTON13;
        }
        else if (keusAppEvents & INWALL_BUTTON14)
        {
            switchPressAction(SWITCH_ID14);
            keusAppEvents ^= INWALL_BUTTON14;
        }
        else if (keusAppEvents & INWALL_BUTTON15)
        {
            switchPressAction(SWITCH_ID15);
            keusAppEvents ^= INWALL_BUTTON15;
        }
        else if (keusAppEvents & INWALL_BUTTON16)
        {
            switchPressAction(SWITCH_ID16);
            keusAppEvents ^= INWALL_BUTTON16;
        }
        else if (keusAppEvents & INWALL_BUTTON17)
        {
            switchPressAction(SWITCH_ID17);
            keusAppEvents ^= INWALL_BUTTON17;
        }

        else if (keusAppEvents & INWALL_UART)
        {
            parseUart();
            // setInterfaceLED(INWALLLED7, 64,0xFF,64);
            // apply_update(DRIVERID1);
            keusAppEvents ^= INWALL_UART;
        }

        else if (keusAppEvents & INWALL_SLEEP_EVT)
        {
            turnOffAllLed();
            workingMode = INWALL_SLEEP;
            keusAppEvents ^= INWALL_SLEEP_EVT;
        }

        // else if (keusAppEvents & KEUS_RETRY_ACK)
        // {
        //     sendSwitchPressInfoAgain();
        //     keusAppEvents ^= KEUS_RETRY_ACK;
        // }
    }
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Set HCLK source form HXT and HCLK source divide 1  */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HXT, CLK_HCLK_CLK_DIVIDER(1));

    /* Enable external 12MHz HXT, 32KHz LXT and HIRC */
    CLK_EnableXtalRC(CLK_PWRCTL_HXT_EN_Msk | CLK_PWRCTL_LXT_EN_Msk | CLK_PWRCTL_HIRC_EN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_HXT_STB_Msk | CLK_CLKSTATUS_LXT_STB_Msk | CLK_CLKSTATUS_HIRC_STB_Msk);

    /*  Set HCLK frequency 42MHz */
    CLK_SetCoreClock(42000000);

    /* Select IP clock source */
    CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL1_UART_S_HIRC, CLK_UART_CLK_DIVIDER(1));
    //CLK_SetModuleClock(I2C0_MODULE, 0, 0);
    CLK_SetModuleClock(I2C1_MODULE, 0, 0);
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(TMR1_MODULE);

    /* Enable IP clock */
    CLK_EnableModuleClock(UART1_MODULE);
    //CLK_EnableModuleClock(I2C0_MODULE);
    CLK_EnableModuleClock(I2C1_MODULE);
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0_S_HXT, 0);
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1_S_HXT, 0);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    // /* Set PA multi-function pins for UART0 RXD and TXD  */
    // SYS->PA_H_MFP = (SYS_PA_H_MFP_PA14_MFP_UART0_RX | SYS_PA_H_MFP_PA15_MFP_UART0_TX);

    /* Set PB multi-function pins for UART1 RXD and TXD  */
    SYS->PB_L_MFP = (SYS_PB_L_MFP_PB4_MFP_UART1_RX | SYS_PB_L_MFP_PB5_MFP_UART1_TX);

    /* Set multi function pin for I2C0/I2C1 */
    SYS->PA_H_MFP |= (SYS_PA_H_MFP_PA11_MFP_I2C1_SCL | SYS_PA_H_MFP_PA10_MFP_I2C1_SDA);

    /* Lock protected registers */
    SYS_LockReg();
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    //uint32_t i;

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    inwallInit();

    inwallLoop();
}
