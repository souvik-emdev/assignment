#include "Nano100Series.h"
#include "inwallconstants.h"
#include "inwallbutton.h"
#include "inwalluart.h"
#include <stdio.h>

// pb14 0x
// pb9 0
// pf 0x
// pf1 1x
// pb15 1x
// pb8 0x

// 100 se3bk

void inwallButtonInit(void)
{
    GPIO_SetMode(PC, BIT0, GPIO_PMD_INPUT); //T1 s4
    GPIO_EnableInt(PC, 0, GPIO_INT_FALLING);

    GPIO_SetMode(PE, BIT5, GPIO_PMD_INPUT); //T2 s3
    GPIO_EnableInt(PE, 5, GPIO_INT_FALLING);

    GPIO_SetMode(PB, BIT11, GPIO_PMD_INPUT); //T3 s2
    GPIO_EnableInt(PB, 11, GPIO_INT_FALLING);

    GPIO_SetMode(PB, BIT10, GPIO_PMD_INPUT); //T4 s1
    GPIO_EnableInt(PB, 10, GPIO_INT_FALLING);

    GPIO_SetMode(PB, BIT9, GPIO_PMD_INPUT); //T5 extrn0 s8
    GPIO_EnableInt(PB, 9, GPIO_INT_FALLING);
    //NVIC_EnableIRQ(EINT0_IRQn);

    GPIO_SetMode(PC, BIT11, GPIO_PMD_INPUT); //T6 s7
    GPIO_EnableInt(PC, 11, GPIO_INT_FALLING);

    GPIO_SetMode(PC, BIT10, GPIO_PMD_INPUT); //T7 s6
    GPIO_EnableInt(PC, 10, GPIO_INT_FALLING);

    GPIO_SetMode(PC, BIT9, GPIO_PMD_INPUT); //T8 s5
    GPIO_EnableInt(PC, 9, GPIO_INT_FALLING);

    GPIO_SetMode(PC, BIT8, GPIO_PMD_INPUT); //T9 s12
    GPIO_EnableInt(PC, 8, GPIO_INT_FALLING);

    GPIO_SetMode(PA, BIT15, GPIO_PMD_INPUT); //T10 s11
    GPIO_EnableInt(PA, 15, GPIO_INT_FALLING);

    GPIO_SetMode(PA, BIT14, GPIO_PMD_INPUT); //T11 s10
    GPIO_EnableInt(PA, 14, GPIO_INT_FALLING);

    GPIO_SetMode(PA, BIT13, GPIO_PMD_INPUT); //T12 s9
    GPIO_EnableInt(PA, 13, GPIO_INT_FALLING);

    GPIO_SetMode(PA, BIT12, GPIO_PMD_INPUT); //T13 s16
    GPIO_EnableInt(PA, 12, GPIO_INT_FALLING);

    GPIO_SetMode(PA, BIT1, GPIO_PMD_INPUT); //T14 s15
    GPIO_EnableInt(PA, 1, GPIO_INT_FALLING);

    GPIO_SetMode(PA, BIT2, GPIO_PMD_INPUT); //T15 s14
    GPIO_EnableInt(PA, 2, GPIO_INT_FALLING);

    GPIO_SetMode(PA, BIT3, GPIO_PMD_INPUT); //T16 s13
    GPIO_EnableInt(PA, 3, GPIO_INT_FALLING);

    GPIO_SetMode(PA, BIT4, GPIO_PMD_INPUT); //T17 s17
    GPIO_EnableInt(PA, 4, GPIO_INT_FALLING);

    NVIC_EnableIRQ(GPABC_IRQn);
    NVIC_EnableIRQ(GPDEF_IRQn);

    /* Enable interrupt de-bounce function and select de-bounce sampling cycle time */
    // GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_HCLK, GPIO_DBCLKSEL_1);
    // GPIO_ENABLE_DEBOUNCE(PB, BIT5);
    // GPIO_ENABLE_DEBOUNCE(PE, BIT2);
}

/**
 * @brief       PortA/PortB/PortC IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The PortA/PortB/PortC default IRQ, declared in startup_nano100series.s.
 */
void GPABC_IRQHandler(void)
{
    uint32_t reg;
    /* To check if PB.5 interrupt occurred */
    if (PC->ISRC & BIT0)
    {
        PC->ISRC = BIT0;
        keusAppEvents |= INWALL_BUTTON4;
    }
    else if (PB->ISRC & BIT11)
    {
        PB->ISRC = BIT11;
        keusAppEvents |= INWALL_BUTTON2;
    }
    else if (PB->ISRC & BIT10)
    {
        PB->ISRC = BIT10;
        keusAppEvents |= INWALL_BUTTON1;
    }
    else if (PC->ISRC & BIT11)
    {
        PC->ISRC = BIT11;
        keusAppEvents |= INWALL_BUTTON7;
    }
    else if (PC->ISRC & BIT10)
    {
        PC->ISRC = BIT10;
        keusAppEvents |= INWALL_BUTTON6;
    }
    else if (PC->ISRC & BIT9)
    {
        PC->ISRC = BIT9;
        keusAppEvents |= INWALL_BUTTON5;
    }
    else if (PC->ISRC & BIT8)
    {
        PC->ISRC = BIT8;
        keusAppEvents |= INWALL_BUTTON12;
    }
    else if (PA->ISRC & BIT15)
    {
        PA->ISRC = BIT15;
        keusAppEvents |= INWALL_BUTTON11;
    }

    else if (PA->ISRC & BIT14)
    {
        PA->ISRC = BIT14;
        keusAppEvents |= INWALL_BUTTON10;
    }
    else if (PA->ISRC & BIT13)
    {
        PA->ISRC = BIT13;
        keusAppEvents |= INWALL_BUTTON9;
    }
    else if (PA->ISRC & BIT12)
    {
        PA->ISRC = BIT12;
        keusAppEvents |= INWALL_BUTTON16;
    }
    else if (PA->ISRC & BIT1)
    {
        PA->ISRC = BIT1;
        keusAppEvents |= INWALL_BUTTON15;
    }
    else if (PA->ISRC & BIT2)
    {
        PA->ISRC = BIT2;
        keusAppEvents |= INWALL_BUTTON14;
    }
    else if (PA->ISRC & BIT3)
    {
        PA->ISRC = BIT3;
        keusAppEvents |= INWALL_BUTTON13;
    }
    else if (PA->ISRC & BIT4)
    {
        PA->ISRC = BIT4;
        keusAppEvents |= INWALL_BUTTON17;
    }
    else if (PB->ISRC & BIT9)
    {
        PB->ISRC = BIT9;
        keusAppEvents |= INWALL_BUTTON8;
    }

    else
    {
        /* Un-expected interrupt. Just clear all PORTA, PORTB, PORTC interrupts */
        reg = PA->ISRC;
        PA->ISRC = reg;
        reg = PB->ISRC;
        PB->ISRC = reg;
        reg = PC->ISRC;
        PC->ISRC = reg;
        //printf("Un-expected interrupts. \n");
    }
}

/**
 * @brief       PortD/PortE/PortF IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The PortD/PortE/PortF default IRQ, declared in startup_nano100series.s.
 */
void GPDEF_IRQHandler(void)
{
    uint32_t reg;
    /* To check if PE.2 interrupt occurred */
    if (PE->ISRC & BIT5)
    {
        PE->ISRC = BIT5;
        // PD0 = PD0 ^ 1;
        // printf("PE.2 INT occurred. \n");
        keusAppEvents |= INWALL_BUTTON3;
    }
    else
    {
        /* Un-expected interrupt. Just clear all PORTD, PORTE and PORTF interrupts */
        reg = PD->ISRC;
        PD->ISRC = reg;
        reg = PE->ISRC;
        PE->ISRC = reg;
        reg = PF->ISRC;
        PF->ISRC = reg;
        //printf("Un-expected interrupts. \n");
    }
}

/**
 * @brief       External INT0 IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The External INT0(PA.2) default IRQ, declared in startup_nano100series.s.
 */
// void EINT0_IRQHandler(void)
// {
//     /* For PB.14, clear the INT flag */
//     PB->ISRC = BIT9;
//     // PD0 = PD0 ^ 1;
//     // printf("PB.14 EINT0 occurred. \n");
//     keusAppEvents |= INWALL_BUTTON8;
// }
