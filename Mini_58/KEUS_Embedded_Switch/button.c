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
    // if (debounce_read)
    // {
    // if (P3->INTSRC & BIT2)
    // {
    //     keusAppEvents |= KEUS_BUTTON1;
    // }

    if (P3->INTSRC & BIT1)
    {
        keusAppEvents |= KEUS_BUTTON2;
    }
    /*ak: remove*/
    else if (P3->INTSRC & BIT0)
    {
        keusAppEvents |= KEUS_BUTTON3;
    }
    //timer_int();
    // TIMER_Open(TIMER0, TIMER_ONESHOT_MODE, 10);
    // TIMER_Start(TIMER0);
    // TIMER_EnableInt(TIMER0);
    // NVIC_EnableIRQ(TMR0_IRQn);
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
    //}
    reg = P3->INTSRC;
    P3->INTSRC = reg;
    //debounce_read = 0;
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