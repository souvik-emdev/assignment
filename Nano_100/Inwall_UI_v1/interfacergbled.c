#include "interfacergbled.h"
#include "Nano100Series.h"
#include "inwallconstants.h"
#include "inwallI2C.h"
#include <stdio.h>

const uint8_t slaveID[MAX_NO_OF_DRIVERS] = {DRIVER_SLAVEID_1, DRIVER_SLAVEID_2};

const uint8_t inwallLEDID[RGB_DRIVER_LEDS] = {5, 4, 3, 2, 1, 0, 6, 10, 7, 8, 9, 11, 15, 14, 13, 12, 22, 21, 20, 23, 16, 17, 18, 19};

volatile uint8_t inwallSleepCounter = 0, workingMode = INWALL_WAKE;
uint8_t splashStage = 0;

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

void enable_rgb_drivers(void)
{
    uint8_t arr[] = {0x0, 0x1};
    for (uint8_t i = 0; i < MAX_NO_OF_DRIVERS; i++)
    {
        inwall_I2C_tx(arr, 2, slaveID[i]);
    }
}

void enable_all_output(void)
{
    uint8_t i, arr[37];
    arr[0] = 0x26;
    for (i = 1; i < 37; i++)
    {
        arr[i] = 0x1;
    }
    for (i = 0; i < MAX_NO_OF_DRIVERS; i++)
    {

        // g_au8MasterTxData[2] = 0xFF;
        // g_au8MasterTxData[3] = 0xFF;
        // g_au8MasterTxData[4] = 0xFF;

        // g_u8MasterDataLen = 0;
        // g_u8EndFlag = 0;
        // g_u8RequiredDataLength = 38;

        inwall_I2C_tx(arr, 37, slaveID[i]);

        // /* I2C function to write data to slave */
        // s_I2C1HandlerFn = (I2C_FUNC)I2C_MasterTx;

        // /* I2C as master sends START signal */
        // I2C_SET_CONTROL_REG(I2C1, I2C_STA);

        // /* Wait I2C Tx Finish */
        // while (g_u8EndFlag == 0)
        //     ;
        // while (I2C1->CON & I2C_CON_STOP_Msk)
        //     ;
        // g_u8EndFlag = 0;
    }
}

void apply_update(uint8_t driverID)
{
    //Update all register
    //g_u8DeviceAddr = slaveID[driverID];

    uint8_t arr[2];
    arr[0] = 0x25;
    arr[1] = 0x00;

    // g_u8MasterDataLen = 0;
    // g_u8EndFlag = 0;
    // g_u8RequiredDataLength = 2;
    if (driverID == ALLDRIVERS)
    {
        inwall_I2C_tx(arr, 2, DRIVER_SLAVEID_1);
        inwall_I2C_tx(arr, 2, DRIVER_SLAVEID_2);
    }
    else
        inwall_I2C_tx(arr, 2, slaveID[driverID]);

    // /* I2C function to write data to slave */
    // s_I2C1HandlerFn = (I2C_FUNC)I2C_MasterTx;

    // /* I2C as master sends START signal */
    // I2C_SET_CONTROL_REG(I2C1, I2C_STA);

    // /* Wait I2C Tx Finish */
    // while (g_u8EndFlag == 0)
    //     ;
    // while (I2C1->CON & I2C_CON_STOP_Msk)
    //     ;
    // g_u8EndFlag = 0;
}

void interfaceLEDInit(void)
{
    GPIO_SetMode(PB, BIT14, GPIO_PMD_OUTPUT);
    GPIO_SetMode(PB, BIT13, GPIO_PMD_OUTPUT);

    PB14 = 1;
    PB13 = 1;

    /* Shutdown register to normal operartion */
    enable_rgb_drivers();
    /* Enable PWM to all LEDS */
    enable_all_output();
    /* Apply update */
    apply_update(ALLDRIVERS);
}

void turnOffAllLed(void)
{
    uint8_t i, arr[37];
    arr[0] = 0x1;
    for (i = 1; i < 37; i++)
    {
        arr[i] = 0x0;
    }
    for (i = 0; i < MAX_NO_OF_DRIVERS; i++)
    {
        inwall_I2C_tx(arr, 37, slaveID[i]);
    }
    apply_update(ALLDRIVERS);
}

void inwallSplash(void)
{
    switch (splashStage)
    {
    //Splash turning off LED by sequence
    case 0:
        splashStage++;
        TIMER_Start(TIMER1);
        setInterfaceLED(inwallLEDID[0], 0xFF, 0, 0);

        break;
    case 1:
        setInterfaceLED(inwallLEDID[1], 0xFF, 0, 0);
        setInterfaceLED(inwallLEDID[2], 0xFF, 0, 0);
        setInterfaceLED(inwallLEDID[6], 0xFF, 0, 0);
        splashStage++;
        break;
    case 2:
        setInterfaceLED(inwallLEDID[3], 0, 0xFF, 0);
        setInterfaceLED(inwallLEDID[4], 0, 0xFF, 0);
        setInterfaceLED(inwallLEDID[7], 0, 0xFF, 0);
        setInterfaceLED(inwallLEDID[8], 0, 0xFF, 0);
        setInterfaceLED(inwallLEDID[12], 0, 0xFF, 0);
        splashStage++;
        break;
    case 3:
        setInterfaceLED(inwallLEDID[5], 0, 0, 0xFF);
        setInterfaceLED(inwallLEDID[9], 0, 0, 0xFF);
        setInterfaceLED(inwallLEDID[10], 0, 0, 0xFF);
        setInterfaceLED(inwallLEDID[13], 0, 0, 0xFF);
        setInterfaceLED(inwallLEDID[20], 0, 0, 0xFF);
        splashStage++;
        break;
    case 4:
        setInterfaceLED(inwallLEDID[11], 0, 0, 0xFF);
        setInterfaceLED(inwallLEDID[14], 0, 0, 0xFF);
        setInterfaceLED(inwallLEDID[21], 0, 0, 0xFF);
        splashStage++;
        break;
    case 5:
        setInterfaceLED(inwallLEDID[15], 0xFF, 0, 0);
        setInterfaceLED(inwallLEDID[22], 0xFF, 0, 0);
        splashStage++;
        break;
    case 6:
        setInterfaceLED(inwallLEDID[23], 0xFF, 0, 0);
        splashStage++;
        break;

        //off sequence of leds

    case 7:
        setInterfaceLED(inwallLEDID[0], 0, 0, 0);
        splashStage++;
        break;
    case 8:
        setInterfaceLED(inwallLEDID[1], 0, 0, 0);
        setInterfaceLED(inwallLEDID[2], 0, 0, 0);
        setInterfaceLED(inwallLEDID[6], 0, 0, 0);
        splashStage++;
        break;
    case 9:
        setInterfaceLED(inwallLEDID[3], 0, 0, 0);
        setInterfaceLED(inwallLEDID[4], 0, 0, 0);
        setInterfaceLED(inwallLEDID[7], 0, 0, 0);
        setInterfaceLED(inwallLEDID[8], 0, 0, 0);
        setInterfaceLED(inwallLEDID[12], 0, 0, 0);
        splashStage++;
        break;
    case 10:
        setInterfaceLED(inwallLEDID[5], 0, 0, 0);
        setInterfaceLED(inwallLEDID[9], 0, 0, 0);
        setInterfaceLED(inwallLEDID[10], 0, 0, 0);
        setInterfaceLED(inwallLEDID[13], 0, 0, 0);
        setInterfaceLED(inwallLEDID[20], 0, 0, 0);
        splashStage++;
        break;
    case 11:
        setInterfaceLED(inwallLEDID[11], 0, 0, 0);
        setInterfaceLED(inwallLEDID[14], 0, 0, 0);
        setInterfaceLED(inwallLEDID[21], 0, 0, 0);
        splashStage++;
        break;
    case 12:
        setInterfaceLED(inwallLEDID[15], 0, 0, 0);
        setInterfaceLED(inwallLEDID[22], 0, 0, 0);
        splashStage++;
        break;
    case 13:
        TIMER_Stop(TIMER1);
        splashStage = 0;
        setInterfaceLED(inwallLEDID[23], 0, 0, 0);
        inwallWakeUp();
        break;

    default:
        //TIMER_Stop(TIMER1);

        break;
    }

    //splashStage++;
    apply_update(ALLDRIVERS);
}

void setInterfaceLED(uint8_t ledID, uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t driverid, ledid, arr[2];
    if (ledID > 11)
        driverid = DRIVERID2;
    else
        driverid = DRIVERID1;

    //g_u8DeviceAddr = slaveID[driverid];

    for (uint8_t i = 0; i < 3; i++)
    {

        switch (i)
        {
        case RED:
            ledid = keusRGBDriver[ledID].red;
            arr[1] = red;
            break;
        case GREEN:
            ledid = keusRGBDriver[ledID].green;
            arr[1] = green;
            break;
        case BLUE:
            ledid = keusRGBDriver[ledID].blue;
            arr[1] = blue;
            break;

        default:
            break;
        }

        //ledid += 0x26;
        //Indivisual LED enable

        arr[0] = ledid;

        // g_au8MasterTxData[0] = ledid;
        // g_au8MasterTxData[1] = 0x01;
        // g_au8MasterTxData[2] = 0x01;
        // g_au8MasterTxData[3] = 0x01;
        // g_au8MasterTxData[4] = 0x01;

        // g_u8MasterDataLen = 0;
        // g_u8EndFlag = 0;
        // g_u8RequiredDataLength = 2;

        inwall_I2C_tx(arr, 2, slaveID[driverid]);

        // /* I2C function to write data to slave */
        // s_I2C1HandlerFn = (I2C_FUNC)I2C_MasterTx;

        // /* I2C as master sends START signal */
        // I2C_SET_CONTROL_REG(I2C1, I2C_STA);

        // /* Wait I2C Tx Finish */
        // while (g_u8EndFlag == 0)
        //     ;
        // while (I2C1->CON & I2C_CON_STOP_Msk)
        //     ;
        // g_u8EndFlag = 0;
    }

    //apply_update(driverid);
}