/**************************************************************************//**
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

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/


volatile uint8_t sendI2c = 0;

void TMR0_IRQHandler(void)
{
    PB8 ^= 1;
    
    if(PB10 == 0)
      if(sendI2c ==  0)
        sendI2c = 1;

    // clear timer interrupt flag
    TIMER_ClearIntFlag(TIMER0);

}



void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Set HCLK source form HXT and HCLK source divide 1  */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HXT,CLK_HCLK_CLK_DIVIDER(1));

    /* Enable external 12MHz HXT, 32KHz LXT and HIRC */
    CLK_EnableXtalRC(CLK_PWRCTL_HXT_EN_Msk | CLK_PWRCTL_LXT_EN_Msk | CLK_PWRCTL_HIRC_EN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_HXT_STB_Msk | CLK_CLKSTATUS_LXT_STB_Msk | CLK_CLKSTATUS_HIRC_STB_Msk);

    /*  Set HCLK frequency 42MHz */
    CLK_SetCoreClock(42000000);

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HIRC, CLK_UART_CLK_DIVIDER(1));
    //CLK_SetModuleClock(I2C0_MODULE, 0, 0);
    CLK_SetModuleClock(I2C1_MODULE, 0, 0);
    CLK_EnableModuleClock(TMR0_MODULE);

    /* Enable IP clock */
    CLK_EnableModuleClock(UART0_MODULE);
    //CLK_EnableModuleClock(I2C0_MODULE);
    CLK_EnableModuleClock(I2C1_MODULE);
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0_S_HXT, 0);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PA multi-function pins for UART0 RXD and TXD  */
    SYS->PA_H_MFP = (SYS_PA_H_MFP_PA14_MFP_UART0_RX | SYS_PA_H_MFP_PA15_MFP_UART0_TX);

    /* Set multi function pin for I2C0/I2C1 */
    SYS->PA_H_MFP |= (SYS_PA_H_MFP_PA11_MFP_I2C1_SCL | SYS_PA_H_MFP_PA10_MFP_I2C1_SDA);

    /* Lock protected registers */
    SYS_LockReg();
}



/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main (void)
{
    //uint32_t i;

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    printf("+-------------------------------------------------------+\n");
    printf("|      Nano100 Series I2C Cross Test Sample Code        |\n");
    printf("+-------------------------------------------------------+\n");
    printf("\n");
    printf("..... Press a key to continue ...\n");
//    getchar();

    GPIO_SetMode(PB, BIT14, GPIO_PMD_OUTPUT);
    GPIO_SetMode(PB, BIT13, GPIO_PMD_OUTPUT);
    GPIO_SetMode(PB, BIT8, GPIO_PMD_OUTPUT);
    GPIO_SetMode(PB, BIT10, GPIO_PMD_INPUT);

    PB14 = 1;
    PB13 = 1;

    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 2);

    // Enable timer interrupt
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);


    // Start Timer 0
    TIMER_Start(TIMER0);

    interfaceLEDInit();

    // for (i = 0; i < 0x100; i++)
    // {
    //     g_u8SlvData[i] = 0;
    // }

    /* I2C function to Slave receive/transmit data */
    //s_I2C1HandlerFn = (I2C_FUNC)I2C_SlaveTRx;
    
    
    while(1)
    {
      if(sendI2c)
      {  
    

    printf("Test Loop =>");
    // for (i = 0; i < 0x100; i++)
    // {
        //printf("%d..", i);
        

        //PWM Brigtness register settings

        setInterfaceLED(LED1, RED, 0xFF);
        setInterfaceLED(LED2, RED, 0xFF);
        setInterfaceLED(LED3, RED, 0xFF);
        setInterfaceLED(LED4, RED, 0xFF);
        setInterfaceLED(LED5, RED, 0xFF);
        setInterfaceLED(LED6, RED, 0xFF);
        setInterfaceLED(LED7, RED, 0xFF);
        setInterfaceLED(LED8, RED, 0xFF);
        setInterfaceLED(LED9, RED, 0xFF);
        setInterfaceLED(LED10, RED, 0xFF);
        setInterfaceLED(LED11, RED, 0xFF);
        setInterfaceLED(LED12, RED, 0xFF);
        setInterfaceLED(LED13, RED, 0xFF);
        setInterfaceLED(LED14, RED, 0xFF);
        setInterfaceLED(LED15, RED, 0xFF);
        setInterfaceLED(LED16, RED, 0xFF);
        setInterfaceLED(LED17, RED, 0xFF); //h1 blue in red
        setInterfaceLED(LED18, RED, 0xFF); //h2 blue in red
        setInterfaceLED(LED19, RED, 0xFF); //h3 blue in red
        setInterfaceLED(LED20, RED, 0xFF); //h4 blue in red
        setInterfaceLED(LED21, RED, 0xFF);
        setInterfaceLED(LED22, RED, 0xFF);
        setInterfaceLED(LED23, RED, 0xFF);
        setInterfaceLED(LED24, RED, 0xFF);





    printf("\nTest Completely !!\n");
    sendI2c = 0;
      }
    }
    //while(1);
}



