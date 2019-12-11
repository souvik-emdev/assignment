#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "Mini58Series.h"
#include "button.h"

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

    if (P3->INTSRC & BIT1)
    {
        keusAppEvents |= KEUS_BUTTON2;
    }
    if (P3->INTSRC & BIT0)
    {
        keusAppEvents |= KEUS_BUTTON3;
    }
    reg = P3->INTSRC;
    P3->INTSRC = reg;
}

/**
 * @brief       External INT0 IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The External INT0(P3.2) default IRQ, declared in startup_Mini58.s.
 */
void EINT0_IRQHandler(void)
{
    /* For P3.2, clear the INT flag */
    P3->INTSRC = BIT2;
    keusAppEvents |= KEUS_BUTTON1;
}

void EINT1_IRQHandler(void)
{
    //uint32_t reg;
    /* For P5.2, clear the INT flag */
    P5->INTSRC = BIT2;
    keusAppEvents |= KEUS_BUTTON4;
    // reg = P5->INTSRC;
    // P5->INTSRC = reg;
}

void keus_button_init(void)
{
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
}