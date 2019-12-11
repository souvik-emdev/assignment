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

#define LED_HIGH 0
#define LED_LOW 1

#define LED1 P22
#define LED2 P23
#define LED3 P24
#define LED4 P25

#define KEUS_BUTTON1 0x1
#define KEUS_BUTTON2 0x2
#define KEUS_BUTTON3 0x4
#define KEUS_BUTTON4 0x8
#define KEUS_UART 0x10

#define KEUS_PWM_CHL3   3

//uint8_t uartStr[] = {'T', 'o', 'g', 'g', 'l', 'e', 'd', 0x0A};
uint8_t tester = 0;

//volatile uint8_t debounce_read = 1;
//volatile uint8_t pwm_toggle = 1;

extern void toggleLed(uint8_t);
extern void parseUart(void);

uint32_t keusAppEvents;

// void TMR0_IRQHandler(void)
// {
//   //debounce_read = 1;
//   //P23 = ~P23;
//   TIMER_ClearIntFlag(TIMER0);
// }

void application_init(void)
{
  /* Enable UART, PWM and setup GPIO pins and interrupt */

  /* Set P0 multi-function pins for UART RXD and TXD and enable interrupt */

  /* ak: should be called in keusuart.c as keusUartInit();*/
  SYS->P1_MFP &= ~(SYS_MFP_P12_Msk | SYS_MFP_P13_Msk);
  SYS->P1_MFP |= (SYS_MFP_P12_UART0_RXD | SYS_MFP_P13_UART0_TXD);
  UART_Open(UART0, 115200);
  UART_ENABLE_INT(UART0, UART_INTEN_RDAIEN_Msk);
  NVIC_EnableIRQ(UART0_IRQn);

  // Setup GPIO outputs
  // SYS->P2_MFP &= ~SYS_MFP_P22_Msk;
  // SYS->P2_MFP |= SYS_MFP_P22_GPIO;
  // GPIO_SetMode(P2, BIT2, GPIO_MODE_OUTPUT);
  // P22 = LED_LOW;

  // SYS->P2_MFP &= ~SYS_MFP_P23_Msk;
  // SYS->P2_MFP |= SYS_MFP_P23_GPIO;
  // GPIO_SetMode(P2, BIT3, GPIO_MODE_OUTPUT);
  // P23 = LED_LOW;

  // SYS->P2_MFP &= ~SYS_MFP_P24_Msk;
  // SYS->P2_MFP |= SYS_MFP_P24_GPIO;
  // GPIO_SetMode(P2, BIT4, GPIO_MODE_OUTPUT);
  // P24 = LED_LOW;

  // SYS->P2_MFP &= ~SYS_MFP_P25_Msk;
  // SYS->P2_MFP |= SYS_MFP_P25_GPIO;
  // GPIO_SetMode(P2, BIT5, GPIO_MODE_OUTPUT);
  // P25 = LED_LOW;

  // Setup GPIO inputs and interrupt
  SYS->P3_MFP &= ~SYS_MFP_P32_Msk;
  SYS->P3_MFP |= SYS_MFP_P32_GPIO;
  GPIO_SetMode(P3, BIT2, GPIO_MODE_INPUT);
  GPIO_EnableInt(P3, 2, GPIO_INT_RISING);
  NVIC_EnableIRQ(EINT0_IRQn);

  GPIO_SetMode(P5, BIT2, GPIO_MODE_INPUT);
  GPIO_EnableEINT1(P5, 2, GPIO_INT_RISING);
  NVIC_EnableIRQ(EINT1_IRQn);

  SYS->P3_MFP &= ~SYS_MFP_P31_Msk;
  SYS->P3_MFP |= SYS_MFP_P31_GPIO;
  GPIO_SetMode(P3, BIT1, GPIO_MODE_INPUT);
  GPIO_EnableInt(P3, 1, GPIO_INT_RISING);
  NVIC_EnableIRQ(GPIO234_IRQn);

  SYS->P3_MFP &= ~SYS_MFP_P30_Msk;
  SYS->P3_MFP |= SYS_MFP_P30_GPIO;
  GPIO_SetMode(P3, BIT0, GPIO_MODE_INPUT);
  GPIO_EnableInt(P3, 0, GPIO_INT_RISING);
  //NVIC_EnableIRQ(GPIO234_IRQn);

  /* Enable interrupt de-bounce function and select de-bounce sampling cycle time */
  GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_HCLK, GPIO_DBCTL_DBCLKSEL_256); //ak: find actual debounce value
  GPIO_ENABLE_DEBOUNCE(P3, BIT2);
  GPIO_ENABLE_DEBOUNCE(P3, BIT1);
  GPIO_ENABLE_DEBOUNCE(P3, BIT0);
  GPIO_ENABLE_DEBOUNCE(P5, BIT4);

  /* Set P2 multi-function pin for PWM Channel 0  */
  // SYS->P2_MFP &= ~SYS_MFP_P22_Msk;
  // SYS->P2_MFP |= SYS_MFP_P22_PWM0_CH0;

  // SYS->P2_MFP &= ~SYS_MFP_P23_Msk;
  // SYS->P2_MFP |= SYS_MFP_P23_PWM0_CH1;

  // SYS->P2_MFP &= ~SYS_MFP_P24_Msk;
  // SYS->P2_MFP |= SYS_MFP_P24_PWM0_CH2;

  // SYS->P2_MFP &= ~SYS_MFP_P25_Msk;
  // SYS->P2_MFP |= SYS_MFP_P25_PWM0_CH3;



  // Start pwm
  PWM_ConfigOutputChannel(PWM, 0, 900, 100);
  PWM_ConfigOutputChannel(PWM, 1, 900, 100);
  PWM_ConfigOutputChannel(PWM, 2, 900, 100);
  PWM_ConfigOutputChannel(PWM, KEUS_PWM_CHL3, 900, 100);
  // Enable output of all PWM channel 0
  PWM_EnableOutput(PWM, 1);
  PWM_EnableOutput(PWM, 2);
  PWM_EnableOutput(PWM, 4);
  PWM_EnableOutput(PWM, (1 << KEUS_PWM_CHL3)); //ak: update bit shifting for other pwms
  PWM_Start(PWM, 0x1);
  PWM_Start(PWM, 0x2);
  PWM_Start(PWM, 0x4);
  PWM_Start(PWM, (1 << KEUS_PWM_CHL3));
}

void KEUS_init(void)
{

  application_init();

  printf("KEUS INIT");// ak: do not use printf

  while (1)
  {
    if (keusAppEvents)
    {
      tester++;
    }
    else continue;

    if (keusAppEvents & KEUS_BUTTON1)
    {
      toggleLed(1);  //ak: LED id instead of 1
      keusAppEvents ^= KEUS_BUTTON1;
    }
    else if (keusAppEvents & KEUS_BUTTON2)
    {
      toggleLed(2);
      keusAppEvents ^= KEUS_BUTTON2;
    }
    else if (keusAppEvents & KEUS_BUTTON3)
    {
      toggleLed(3);
      keusAppEvents ^= KEUS_BUTTON3;
    }
    else if (keusAppEvents & KEUS_BUTTON4)
    {
      toggleLed(4);
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
