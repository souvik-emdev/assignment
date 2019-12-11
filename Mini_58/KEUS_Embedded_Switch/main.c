/******************************************************************************
* @file     main.c
* @version  V1.00
* $Date: 07/12/19 10:37a
* @brief    Controlling the state of 4 GPIOs with 4 push-buttons and UART.
*           Whenever state will change by button press, inform that via
*           UART.
*
* @note
* Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "Mini58Series.h"

#define PLL_CLOCK 50000000

#define KEUS_BUTTON1 0x1
#define KEUS_BUTTON2 0x2
#define KEUS_BUTTON3 0x4
#define KEUS_BUTTON4 0x8
#define KEUS_UART 0x10

#define KEUS_PWM_CHL0 0
#define KEUS_PWM_CHL1 1
#define KEUS_PWM_CHL2 2
#define KEUS_PWM_CHL3 3
#define KEUS_PWM_FREQUENCY 900
#define KEUS_STARTUP_DUTY 100

#define LEDID1 1
#define LEDID2 2
#define LEDID3 3
#define LEDID4 4

uint8_t tester = 0;


extern void toggleLed(uint8_t);
extern void parseUart(void);
extern void keus_uart_init(void);
extern void keus_button_init(void);
extern void keus_config_switch_init(void);

uint32_t keusAppEvents;


void application_init(void)
{
  // Start pwm
  PWM_ConfigOutputChannel(PWM, KEUS_PWM_CHL0, KEUS_PWM_FREQUENCY, KEUS_STARTUP_DUTY);
  PWM_ConfigOutputChannel(PWM, KEUS_PWM_CHL1, KEUS_PWM_FREQUENCY, KEUS_STARTUP_DUTY);
  PWM_ConfigOutputChannel(PWM, KEUS_PWM_CHL2, KEUS_PWM_FREQUENCY, KEUS_STARTUP_DUTY);
  PWM_ConfigOutputChannel(PWM, KEUS_PWM_CHL3, KEUS_PWM_FREQUENCY, KEUS_STARTUP_DUTY);
  // Enable output of all PWM channel 0
  PWM_EnableOutput(PWM, (1 << KEUS_PWM_CHL0));
  PWM_EnableOutput(PWM, (1 << KEUS_PWM_CHL1));
  PWM_EnableOutput(PWM, (1 << KEUS_PWM_CHL2));
  PWM_EnableOutput(PWM, (1 << KEUS_PWM_CHL3));
  PWM_Start(PWM, (1 << KEUS_PWM_CHL0));
  PWM_Start(PWM, (1 << KEUS_PWM_CHL1));
  PWM_Start(PWM, (1 << KEUS_PWM_CHL2));
  PWM_Start(PWM, (1 << KEUS_PWM_CHL3));
}

void KEUS_init(void)
{

  keus_uart_init();
  keus_button_init();
  keus_config_switch_init();
  application_init();

  while (1)
  {
    if (keusAppEvents)
    {
      tester++;
    }
    else continue;

    if (keusAppEvents & KEUS_BUTTON1)
    {
      toggleLed(LEDID1);
      keusAppEvents ^= KEUS_BUTTON1;
    }
    else if (keusAppEvents & KEUS_BUTTON2)
    {
      toggleLed(LEDID2);
      keusAppEvents ^= KEUS_BUTTON2;
    }
    else if (keusAppEvents & KEUS_BUTTON3)
    {
      toggleLed(LEDID3);
      keusAppEvents ^= KEUS_BUTTON3;
    }
    else if (keusAppEvents & KEUS_BUTTON4)
    {
      toggleLed(LEDID4);
      keusAppEvents ^= KEUS_BUTTON4;
    }

    else if (keusAppEvents & KEUS_UART)
    {
      parseUart();
      keusAppEvents ^= KEUS_UART;
    }
  }

}

void SYS_Init(void)
{
  /*---------------------------------------------------------------------------------------------------------*/
  /* Init System Clock                                                                                       */
  /*---------------------------------------------------------------------------------------------------------*/

  /* Unlock protected registers */
  SYS_UnlockReg();

  /* Set P5 multi-function pins for crystal output/input */
  SYS->P5_MFP &= ~(SYS_MFP_P50_Msk | SYS_MFP_P51_Msk);
  SYS->P5_MFP |= (SYS_MFP_P50_XT1_IN | SYS_MFP_P51_XT1_OUT);

  /*
  Peower control and main clock
  */
  // Clearing all bits of power control register
  CLK->PWRCTL &= ~(0x2ff);
  /* Enable External XTAL (4~24 MHz) */
  CLK->PWRCTL |= (CLK_PWRCTL_XTL12M << CLK_PWRCTL_XTLEN_Pos);
  //CLK->PWRCTL |= (CLK_PWRCTL_LIRCEN_Msk);

  /* Waiting for 12MHz clock ready */
  CLK_WaitClockReady(CLK_STATUS_XTLSTB_Msk);

  /* 
  Switch HCLK and Sys tick clock source to XTAL 
  */
  CLK->CLKSEL0 &= ~CLK_CLKSEL0_HCLKSEL_Msk;
  CLK->CLKSEL0 |= (CLK_CLKSEL0_HCLKSEL_XTAL << CLK_CLKSEL0_HCLKSEL_Pos);
  CLK->CLKSEL0 &= ~CLK_CLKSEL0_STCLKSEL_Msk;
  CLK->CLKSEL0 |= (CLK_CLKSEL0_STCLKSEL_XTAL << CLK_CLKSEL0_STCLKSEL_Pos);
  /*
  Peripheral clock select
  */
  // clearing APBCLK unreserved register
  CLK->APBCLK &= ~(0xd073134d);
  /* Enable peripheral clocks */
  //    CLK->APBCLK |= CLK_APBCLK_WDTCKEN_Msk;
  CLK->APBCLK |= CLK_APBCLK_TMR0CKEN_Msk;
  //CLK->APBCLK |= CLK_APBCLK_TMR1CKEN_Msk;
  //    CLK->APBCLK |= CLK_APBCLK_CLKOCKEN_Msk; // clock divider
  //    CLK->APBCLK |= CLK_APBCLK_I2C0CKEN_Msk;
  //    CLK->APBCLK |= CLK_APBCLK_I2C1CKEN_Msk;
  //    CLK->APBCLK |= CLK_APBCLK_SPICKEN_Msk;
  CLK->APBCLK |= CLK_APBCLK_UART0CKEN_Msk;
  //    CLK->APBCLK |= CLK_APBCLK_UART1CKEN_Msk;
  CLK->APBCLK |= CLK_APBCLK_PWMCH01CKEN_Msk;
  CLK->APBCLK |= CLK_APBCLK_PWMCH23CKEN_Msk;
  //    CLK->APBCLK |= CLK_APBCLK_PWMCH45CKEN_Msk;
  //    CLK->APBCLK |= CLK_APBCLK_ADCCKEN_Msk;
  //    CLK->APBCLK |= CLK_APBCLK_ACMPCKEN_Msk;

  /*
  Peripheral clock source 1
  */
  // clearing peripheral clock sources
  CLK->CLKSEL1 &= ~(0xf300773f);
  // Select peripheral clock sources
  CLK->CLKSEL1 |= CLK_CLKSEL1_UARTSEL_XTAL;
  CLK->CLKSEL1 |= CLK_CLKSEL1_TMR0SEL_XTAL;
  //CLK->CLKSEL1 |= CLK_CLKSEL1_TMR1SEL_XTAL;
  //CLK->CLKSEL1 |= CLK_CLKSEL1_TMR1SEL_HCLK;
  //CLK->CLKSEL1 |= CLK_CLKSEL1_WDTSEL_LIRC;
  CLK->CLKSEL1 |= CLK_CLKSEL1_PWMCH01SEL_HCLK;
  CLK->CLKSEL1 |= CLK_CLKSEL1_PWMCH23SEL_HCLK;
  //    CLK->CLKSEL1 |= CLK_CLKSEL1_SPISEL_XTAL;

  /*
  Peripheral clock source 2
  */
  //CLK->CLKSEL2 &= ~CLK_CLKSEL2_PWMCH45SEL_Msk;
  //CLK->CLKSEL2 |= CLK_CLKSEL2_PWMCH45SEL_HCLK;

  /*
  Peripheral clock divider
  */
  // clearing peripheral clock dividers
  CLK->CLKDIV &= ~(0x00ff0f0f);
  //  Select peripheral clock divider
  //    CLK->CLKDIV |=CLK_CLKDIV_ADC(x);
  //    CLK->CLKDIV |=CLK_CLKDIV_UART(1);
  //    CLK->CLKDIV |=CLK_CLKDIV_HCLK(x);

  /* Setup SPI multi-function pin */
  //SYS->P0_MFP |= SYS_MFP_P04_SPI0_SS | SYS_MFP_P05_SPI0_MOSI | SYS_MFP_P06_SPI0_MISO | SYS_MFP_P07_SPI0_CLK;

  /* Set core clock as PLL_CLOCK from PLL */
  //CLK_SetCoreClock(PLL_CLOCK);

  /* Update System Core Clock */
  /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and CycylesPerUs automatically. */
  SystemCoreClockUpdate();

  /* Set P2 multi-function pin for PWM Channel 0  */
  // Start pwm
  // PWM_ConfigOutputChannel(PWM, 0, 38000, 50);
  // Enable output of all PWM channel 0
  // PWM_EnableOutput(PWM, 1);
  // PWM_Start(PWM, 0x1);
  //SYS->P2_MFP = SYS_MFP_P22_PWM0_CH0;

  /* Lock protected registers */
  SYS_LockReg();
}

int main(void)
{

  /* Init System, IP clock and multi-function I/O
  In the end of SYS_Init() will issue SYS_LockReg()
  to lock protected register. If user want to write
  protected register, please issue SYS_UnlockReg()
  to unlock protected register if necessary */
  SYS_Init();

  KEUS_init();

  /* Configure P2.4 as Input mode and enable interrupt by rising edge trigger */

  //uint8_t uartReply[] = {'D', 'o', 'n', 'e', '!', 0x0A};

  /* Enable interrupt de-bounce function and select de-bounce sampling cycle time */
  // GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_HCLK, GPIO_DBCTL_DBCLKSEL_512);
  // GPIO_ENABLE_DEBOUNCE(P2, BIT4);

  // Set timer 1 shot with 10HZ
  //  TIMER_Open(TIMER0, TIMER_ONESHOT_MODE, 10);

  // Update prescale value
  //TIMER_SET_PRESCALE_VALUE(TIMER0, 0);
  // Start Timer 0
  //  TIMER_Start(TIMER0);
  // Enable timer interrupt
  //  TIMER_EnableInt(TIMER0);
  //  NVIC_EnableIRQ(TMR0_IRQn);

  // while (1)
  // {

  //   // if (P24 == 0) {

  //   //   TIMER_Delay(TIMER0, 250000);
  //   // }

  //   if (message_received)
  //   {
  //     message_received = 0;
  //     //P23 = g0_u8RecData[0];
  //     UART_Write(UART0, uartStr, 6);
  //     // parse_data();
  //     // print_ir_out2();
  //     // transmit();
  //   }
  // }
}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/
