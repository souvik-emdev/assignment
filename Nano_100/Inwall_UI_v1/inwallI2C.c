#include "Nano100Series.h"
#include "inwallconstants.h"
#include "inwallI2C.h"
#include <stdio.h>

typedef void (*I2C_FUNC)(uint32_t u32Status);

uint8_t g_u8DeviceAddr;
uint8_t g_au8MasterTxData[40];
volatile uint8_t g_u8MasterDataLen;
volatile uint8_t g_u8EndFlag = 0;

uint8_t g_u8RequiredDataLength = 0;

volatile I2C_FUNC s_I2C1HandlerFn = NULL;

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C1 IRQ Handler                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void I2C1_IRQHandler(void)
{
    uint32_t u32Status;

    // clear interrupt flag
    I2C1->INTSTS |= I2C_INTSTS_INTSTS_Msk;

    u32Status = I2C_GET_STATUS(I2C1);
    if (I2C_GET_TIMEOUT_FLAG(I2C1))
    {
        /* Clear I2C1 Timeout Flag */
        I2C_ClearTimeoutFlag(I2C1);
    }
    else
    {
        if (s_I2C1HandlerFn != NULL)
            s_I2C1HandlerFn(u32Status);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C1 Tx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterTx(uint32_t u32Status)
{
    if (u32Status == 0x08) /* START has been transmitted */
    {
        I2C_SET_DATA(I2C1, g_u8DeviceAddr << 1); /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C1, I2C_SI);
    }
    else if (u32Status == 0x18) /* SLA+W has been transmitted and ACK has been received */
    {
        I2C_SET_DATA(I2C1, g_au8MasterTxData[g_u8MasterDataLen++]);
        I2C_SET_CONTROL_REG(I2C1, I2C_SI);
    }
    else if (u32Status == 0x20) /* SLA+W has been transmitted and NACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C1, I2C_STA | I2C_STO | I2C_SI);
    }
    else if (u32Status == 0x28) /* DATA has been transmitted and ACK has been received */
    {
        if (g_u8MasterDataLen != g_u8RequiredDataLength)
        {
            I2C_SET_DATA(I2C1, g_au8MasterTxData[g_u8MasterDataLen++]);
            I2C_SET_CONTROL_REG(I2C1, I2C_SI);
        }
        else
        {
            I2C_SET_CONTROL_REG(I2C1, I2C_STO | I2C_SI);
            g_u8EndFlag = 1;
        }
    }
    else
    {
        /* TO DO */
        printf("Status 0x%x is NOT processed\n", u32Status);
    }
}

void I2C1_Init(void)
{
    /* Open I2C1 and set clock to 100k */
    I2C_Open(I2C1, 100000);

    /* Get I2C1 Bus Clock */
    //printf("I2C1 clock %d Hz\n", I2C_GetBusClockFreq(I2C1));
//I2C_EnableTimeout(I2C1, 100);
    I2C_EnableInt(I2C1);
    NVIC_EnableIRQ(I2C1_IRQn);
   // TIMER_Start(TIMER1);
}

void inwall_I2C_tx(uint8_t arr[], uint8_t datalen, uint8_t slaveID)
{
    g_u8MasterDataLen = 0;
    g_u8EndFlag = 0;
    g_u8RequiredDataLength = datalen;
    g_u8DeviceAddr = slaveID;

    for (uint8_t i = 0; i < datalen; i++)
    {
        g_au8MasterTxData[i] = arr[i];
    }

    /* I2C function to write data to slave */
    s_I2C1HandlerFn = (I2C_FUNC)I2C_MasterTx;

    /* I2C as master sends START signal */
    I2C_SET_CONTROL_REG(I2C1, I2C_STA);

    /* Wait I2C Tx Finish */
    while (g_u8EndFlag == 0)
        ;
    while (I2C1->CON & I2C_CON_STOP_Msk)
        ;
    g_u8EndFlag = 0;
}