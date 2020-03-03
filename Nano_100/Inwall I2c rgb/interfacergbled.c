#include "interfacergbled.h"
#include "Nano100Series.h"
#include <stdio.h>

const uint8_t slaveID[MAX_NO_OF_DRIVERS] = {DRIVER_SLAVEID_1, DRIVER_SLAVEID_2};

const rgbdriver keusRGBDriver[] = {
    {3, 4, 5},    //LD2 0
    {6, 7, 8},    //LD3 1
    {9, 10, 11},  //LD4 2
    {12, 13, 14}, //LD5 3
    {15, 16, 17}, //LD6 4
    {18, 19, 20}, //LD7 5
    {21, 22, 23}, //LD8 6
    {27, 28, 29}, //LD9 7
    {30, 31, 32}, //LD10 8
    {33, 34, 35}, //LD11 9
    {24, 25, 26}, //LD13 10
    {36, 1, 2},   //LD14 11

    //Driver 2

    {3, 4, 5},    //LD12 12
    {6, 7, 8},    //LD15 13
    {9, 10, 11},  //LD16 14
    {12, 13, 14}, //LD17 15
    {15, 16, 17}, //LD18 16
    {18, 19, 20}, //LD19 17
    {21, 22, 23}, //LD20 18
    {24, 25, 26}, //LD21 19
    {30, 31, 32}, //LD22 20
    {33, 34, 35}, //LD23 21
    {36, 1, 2},   //LD24 22
    {27, 28, 29}  //LD25 23
};

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
    printf("I2C1 clock %d Hz\n", I2C_GetBusClockFreq(I2C1));

    I2C_EnableInt(I2C1);
    NVIC_EnableIRQ(I2C1_IRQn);
}

void enable_rgb_drivers(void)
{
    for (uint8_t i = 0; i < MAX_NO_OF_DRIVERS; i++)
    {
        g_u8DeviceAddr = slaveID[i];
        g_au8MasterTxData[0] = 0x0;
        g_au8MasterTxData[1] = 0x1;

        g_u8MasterDataLen = 0;
        g_u8EndFlag = 0;
        g_u8RequiredDataLength = 2;

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
}

void enable_all_pwm(void)
{
  uint8_t i;
    for (i = 1; i < 37; i++)
    {
        g_au8MasterTxData[i] = 0xFF;
    }
    for (i = 0; i < MAX_NO_OF_DRIVERS; i++)
    {
        g_u8DeviceAddr = slaveID[i];
        g_au8MasterTxData[0] = 0x1;

        // g_au8MasterTxData[2] = 0xFF;
        // g_au8MasterTxData[3] = 0xFF;
        // g_au8MasterTxData[4] = 0xFF;

        g_u8MasterDataLen = 0;
        g_u8EndFlag = 0;
        g_u8RequiredDataLength = 38;

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
}

void apply_update(uint8_t driverID)
{
    //Update all register
    g_u8DeviceAddr = slaveID[driverID];

    g_au8MasterTxData[0] = 0x25;
    g_au8MasterTxData[1] = 0x00;

    g_u8MasterDataLen = 0;
    g_u8EndFlag = 0;
    g_u8RequiredDataLength = 2;

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

void interfaceLEDInit(void)
{
    /* Configure I2C1 as master */
    I2C1_Init();
    /* Shutdown register to normal operartion */
    enable_rgb_drivers();
    /* Enable PWM to all LEDS */
    enable_all_pwm();
    /* Apply update */
    apply_update(DRIVERID1);
    apply_update(DRIVERID2);
}

void setInterfaceLED(uint8_t ledID, uint8_t colour, uint8_t brightness)
{
    uint8_t driverid;
    if (ledID > 11)
        driverid = DRIVERID2;
    else
        driverid = DRIVERID1;
    
    g_u8DeviceAddr = slaveID[driverid];

    uint8_t ledid;

    switch (colour)
    {
    case RED:
        ledid = keusRGBDriver[ledID].red - 1;
        break;
    case GREEN:
        ledid = keusRGBDriver[ledID].green - 1;
        break;
    case BLUE:
        ledid = keusRGBDriver[ledID].blue - 1;
        break;

    default:
        break;
    }

    ledid += 0x26;
    //Indivisual LED enable
    g_au8MasterTxData[0] = ledid;
    g_au8MasterTxData[1] = 0x01;
    // g_au8MasterTxData[2] = 0x01;
    // g_au8MasterTxData[3] = 0x01;
    // g_au8MasterTxData[4] = 0x01;

    g_u8MasterDataLen = 0;
    g_u8EndFlag = 0;
    g_u8RequiredDataLength = 2;

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

    apply_update(driverid);
}