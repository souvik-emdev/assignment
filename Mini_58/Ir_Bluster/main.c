/******************************************************************************
* @file     main.c
* @version  V1.00
* $Revision: 3 $
* $Date: 15/05/28 10:37a $
* @brief    Use pin P2.4 to as switch input and 2.3 as LED output.
*           Pressing Switch changes LED state.
*           LED state can be changed from UART msg.
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
#include "irBluster.h"
#include "irGenerator.h"

#define PLL_CLOCK 50000000

#define LED_HIGH 0
#define LED_LOW 1

#define KEUS_UART_MSG_INITIATOR 0x28
#define KEUS_UART_MSG_TERMINATOR 0x29
#define KEUS_UART_BUFFER 32

uint8_t ir_out2[12]; //final decoded array which contain both LSB8 and MSB


volatile uint8_t message_received = 0;
volatile uint8_t g0_u8RecData[KEUS_UART_BUFFER] = {0};
volatile uint8_t g0_bufferCounter = 0;
volatile uint8_t g0_shouldSaveToBuffer = 0;
volatile uint8_t g0_endMessageIndex = 0;
uint8_t uartStr[] = {'T', 'o', 'g', 'g', 'l', 'e', 'd', 0x0A};

volatile uint8_t debounce_read = 1;
//volatile uint8_t pwm_toggle = 1;

void TMR0_IRQHandler(void)
{
  debounce_read = 1;
  //P23 = ~P23;
  TIMER_ClearIntFlag(TIMER0);
}

/**
 * @brief       Port2/Port3/Port4 IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The Port2/Port3/Port4 default IRQ, declared in startup_Mini58.s.
 */
void GPIO234_IRQHandler(void)
{
  uint32_t reg;
  if (debounce_read)
  {
    P23 = ~P23;
    UART_Write(UART0, uartStr, 8);
    //timer_int();
    TIMER_Open(TIMER0, TIMER_ONESHOT_MODE, 10);
    TIMER_Start(TIMER0);
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);
    // if (pwm_toggle)
    // {
    //   // Start pwm
    //   PWM_ConfigOutputChannel(PWM, 0, 38000, 50);
    //   // Enable output of all PWM channel 0
    //   PWM_EnableOutput(PWM, 1);
    //   PWM_Start(PWM, 0x1);
    //   pwm_toggle = 0;
    // }
    // else
    // {
    //   PWM_Stop(PWM, 0x1);
    //   pwm_toggle = 1;
    // }
  }
  reg = P2->INTSRC;
  P2->INTSRC = reg;
  debounce_read = 0;
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
  //    CLK->APBCLK |= CLK_APBCLK_PWMCH23CKEN_Msk;
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
  //CLK->CLKSEL1 |= CLK_CLKSEL1_PWMCH23SEL_HCLK;
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

  /* Set P0 multi-function pins for UART RXD and TXD */
  //SYS->P1_MFP &= ~(SYS_MFP_P12_Msk | SYS_MFP_P13_Msk);
  //SYS->P1_MFP |= (SYS_MFP_P12_UART0_RXD | SYS_MFP_P13_UART0_TXD);

  /* Setup SPI multi-function pin */
  //SYS->P0_MFP |= SYS_MFP_P04_SPI0_SS | SYS_MFP_P05_SPI0_MOSI | SYS_MFP_P06_SPI0_MISO | SYS_MFP_P07_SPI0_CLK;

  /* Set core clock as PLL_CLOCK from PLL */
  //CLK_SetCoreClock(PLL_CLOCK);

  /* Update System Core Clock */
  /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and CycylesPerUs automatically. */
  SystemCoreClockUpdate();

  /* Set P2 multi-function pin for PWM Channel 0  */
  // Start pwm
  PWM_ConfigOutputChannel(PWM, 0, 38000, 50);
  // Enable output of all PWM channel 0
  PWM_EnableOutput(PWM, 1);
  PWM_Start(PWM, 0x1);
  //SYS->P2_MFP = SYS_MFP_P22_PWM0_CH0;

  /* Lock protected registers */
  SYS_LockReg();
}

void UART0_IRQHandler(void)
{
  uint8_t u8InChar = 0x73;
  uint32_t u32IntSts = UART0->INTSTS;

  if (u32IntSts & UART_INTSTS_RDAINT_Msk)
  {
    /* Get all the input characters */
    while (UART_IS_RX_READY(UART0))
    {

      /* Get the character from UART Buffer */
      u8InChar = UART_READ(UART0); /* Rx trigger level is 1 byte*/

      if (u8InChar == KEUS_UART_MSG_INITIATOR && g0_endMessageIndex == 0)
      {
        g0_shouldSaveToBuffer = 1;
        g0_bufferCounter = 0;
      }
      else if (g0_endMessageIndex == 0 && g0_shouldSaveToBuffer)
      {
        g0_endMessageIndex = u8InChar - 1;
      }
      else if (u8InChar == KEUS_UART_MSG_TERMINATOR && g0_endMessageIndex == g0_bufferCounter)
      {
        g0_shouldSaveToBuffer = 0;
        g0_endMessageIndex = 0;

        message_received = g0_bufferCounter;
      }
      else if (g0_bufferCounter > g0_endMessageIndex)
      {
        g0_shouldSaveToBuffer = 0;
        g0_endMessageIndex = 0;
      }
      else if (g0_shouldSaveToBuffer)
        g0_u8RecData[g0_bufferCounter++] = u8InChar;
      else
        g0_endMessageIndex = 0;
    }
  }
}

void print_ir_out2()
{
  printf("");
  printf("Decoded remote signal is:");
  for (int i = 0; i < 12; i++)
  {
    printf("%x", ir_out2[i]);
    printf(" ");
  }
  printf("");
}

void parse_data(void)
{
  uint8_t i = 0, comma = 1, temp, fan, hsw, vsw, mode;
  for (i = 0; i < sizeof(g0_u8RecData); i++)
  {
    if (g0_u8RecData[i] != 0x44)
    {
      if (comma == 1)
      {
        temp = g0_u8RecData[i];
      }
      if (comma == 2)
      {
        fan = g0_u8RecData[i];
      }
      if (comma == 3)
      {
        hsw = g0_u8RecData[i];
      }
      if (comma == 4)
      {
        vsw = g0_u8RecData[i];
      }
      if (comma == 5)
      {
        mode = g0_u8RecData[i];
      }
    }
    else if (g0_u8RecData[i] == 0x44)
    {
      comma++;
    }
  }
  ir_generator(temp, fan, hsw, vsw, mode);
  printf("\nTemp: %d \n", temp);
  printf("Fan: %d \n", fan);
  printf("HSW: %d \n", hsw);
  printf("Vsw: %d \n", vsw);
  printf("mode: %d \n", mode);
}

int main(void)
{

  /* Init System, IP clock and multi-function I/O
  In the end of SYS_Init() will issue SYS_LockReg()
  to lock protected register. If user want to write
  protected register, please issue SYS_UnlockReg()
  to unlock protected register if necessary */
  SYS_Init();

  /* Set P0 multi-function pins for UART RXD and TXD */
  SYS->P1_MFP &= ~(SYS_MFP_P12_Msk | SYS_MFP_P13_Msk);
  SYS->P1_MFP |= (SYS_MFP_P12_UART0_RXD | SYS_MFP_P13_UART0_TXD);
  UART_Open(UART0, 115200);
  UART_ENABLE_INT(UART0, UART_INTEN_RDAIEN_Msk);
  NVIC_EnableIRQ(UART0_IRQn);

  SYS->P3_MFP &= ~SYS_MFP_P32_Msk;
  SYS->P3_MFP |= SYS_MFP_P32_GPIO;
  GPIO_SetMode(P3, BIT2, GPIO_MODE_OUTPUT);
  P32 = 0;

  SYS->P2_MFP &= ~SYS_MFP_P23_Msk;
  SYS->P2_MFP |= SYS_MFP_P23_GPIO;
  GPIO_SetMode(P2, BIT3, GPIO_MODE_OUTPUT);
  P23 = LED_LOW;

  /* Configure P2.4 as Input mode and enable interrupt by rising edge trigger */
  SYS->P2_MFP &= ~SYS_MFP_P24_Msk;
  SYS->P2_MFP |= SYS_MFP_P24_GPIO;
  GPIO_SetMode(P2, BIT4, GPIO_MODE_INPUT);
  GPIO_EnableInt(P2, 4, GPIO_INT_RISING);
  NVIC_EnableIRQ(GPIO234_IRQn);

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

  while (1)
  {

    // if (P24 == 0) {

    //   TIMER_Delay(TIMER0, 250000);
    // }

    if (message_received)
    {
      message_received = 0;
      //P23 = g0_u8RecData[0];
      //UART_Write(UART0, uartReply, 6);
      parse_data();
      print_ir_out2();
      transmit();
    }
  }
}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/
