#include "Nano100Series.h"
#include "inwallactions.h"
#include "inwallconstants.h"
#include "inwalluart.h"
#include "inwallI2C.h"
#include "inwalluart.h"
#include "inwallbutton.h"
#include "interfacergbled.h"
#include "inwallcolours.h"
#include <stdio.h>

void setRGB(void);
void taskHandler(void);
void fanHandler(uint8_t operation);
uint8_t commandId, uartDataBuffer[MAX_UART_DATA_BUFFER];

inwallMemoryLayout inwallData;

void fillDefaultData(void)
{
    for (uint8_t i = 0; i < MAX_NO_OF_SCENES; i++)
    {
        inwallData.sceneID[i] = i + 1;
        inwallData.arr_switch[i].state = 0;
        inwallData.arr_switch[i].config = CONFIG_NORMAL;
    }
}

void TMR1_IRQHandler(void)
{
    keusAppEvents |= INWALL_SPLASH_EVT;
    // clear timer interrupt flag
    TIMER_ClearIntFlag(TIMER1);
}

void TMR0_IRQHandler(void)
{
    keusAppEvents |= INWALL_WATCHDOG_EVT;
    if (workingMode)
    {
        inwallSleepCounter++;
        if (inwallSleepCounter >= 15)
        {
            inwallSleepCounter = 0;
            keusAppEvents |= INWALL_SLEEP_EVT;
        }
    }

    // clear timer interrupt flag
    TIMER_ClearIntFlag(TIMER0);
}

void inwallWakeUp(void)
{
    //Set all themeswitch leds white
    for (uint8_t i = 0; i < 12; i++)
    {
        setInterfaceLED(inwallLEDID[i], GENERAL_WHITE, GENERAL_WHITE, GENERAL_WHITE);
    }

    inwallData.arr_switch[0].state ? setInterfaceLED(inwallLEDID[12], SWITCH_1_R, SWITCH_1_G, SWITCH_1_B) : setInterfaceLED(inwallLEDID[12], GENERAL_WHITE, GENERAL_WHITE, GENERAL_WHITE);
    inwallData.arr_switch[1].state ? setInterfaceLED(inwallLEDID[13], SWITCH_2_R, SWITCH_2_G, SWITCH_2_B) : setInterfaceLED(inwallLEDID[13], GENERAL_WHITE, GENERAL_WHITE, GENERAL_WHITE);
    inwallData.arr_switch[2].state ? setInterfaceLED(inwallLEDID[14], SWITCH_3_R, SWITCH_3_G, SWITCH_3_B) : setInterfaceLED(inwallLEDID[14], GENERAL_WHITE, GENERAL_WHITE, GENERAL_WHITE);
    inwallData.arr_switch[3].state ? setInterfaceLED(inwallLEDID[20], SWITCH_4_R, SWITCH_4_G, SWITCH_4_B) : setInterfaceLED(inwallLEDID[20], GENERAL_WHITE, GENERAL_WHITE, GENERAL_WHITE);
    inwallData.arr_switch[4].state ? setInterfaceLED(inwallLEDID[21], SWITCH_5_R, SWITCH_5_G, SWITCH_5_B) : setInterfaceLED(inwallLEDID[21], GENERAL_WHITE, GENERAL_WHITE, GENERAL_WHITE);
    inwallData.arr_switch[5].state ? setInterfaceLED(inwallLEDID[22], SWITCH_6_R, SWITCH_6_G, SWITCH_6_B) : setInterfaceLED(inwallLEDID[22], GENERAL_WHITE, GENERAL_WHITE, GENERAL_WHITE);
    fanHandler(FAN_OPERATION_SET_LEDS);
    setInterfaceLED(inwallLEDID[15], FAN_R, FAN_G, FAN_B);
    setInterfaceLED(inwallLEDID[23], FAN_R, FAN_G, FAN_B);
    apply_update(ALLDRIVERS);
}

void inwallInit(void)
{
    inwalluartinit();
    GPIO_SetMode(PB, BIT8, GPIO_PMD_OUTPUT); //watchdog pin

    //Responsible for toggling watchdog, wake timeout & retry ack msg
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 2);
    // Enable timer interrupt
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);

    //Responsible for showing the splash animation
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 15);
    // Enable timer interrupt
    TIMER_EnableInt(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);

    /* Configure I2C1 as master */
    I2C1_Init();

    interfaceLEDInit();

    inwallButtonInit();

    // Start Timer 0
    TIMER_Start(TIMER0);

    fillDefaultData();

    inwallSplash();
}

void parseUart(void)
{
    uint8_t i = 0, j = 1;
    commandId = g0_u8RecData[0];
    //txnId = g0_u8RecData[1];
    for (i = 0; i < MAX_UART_DATA_BUFFER; i++)
    {
        uartDataBuffer[i] = g0_u8RecData[j];
        j++;
    }

    taskHandler();
}

//Calls the required function as needed by UART received message
void taskHandler(void)
{
    switch (commandId)
    {
    // case TASK_CONFIG_SWITCH:
    //     configSwitch();
    //     break;

    // case TASK_CONFIG_FADETIME:
    //     configFadetime();
    //     break;
    // // case TASK_GET_SWITCH_STATE:
    // //     getSwitchState();
    // //     break;

    // // case TASK_GET_CONFIG:
    // //     getConfig();
    // //     break;
    // case TASK_EXECUTE_SWITCH_STATE:
    //     executeSwitchState();
    //     break;

    // case TASK_UART_RETRY_ACK:
    //     uartAck();
    //     break;
    case SET_RGB:
        setRGB();
        break;

    default:
        break;
    }
}

void setRGB(void)
{ //Example: 2 1 100 200 220 2 50 60 70 3 10 20 30
    uint8_t noOfLED = uartDataBuffer[0];
    uint8_t position = 0, ledId, redState, greenState, blueState, sendReply[5], datalen = 0;

    for (uint8_t i = 0; i < noOfLED; i++)
    {
        ledId = inwallLEDID[uartDataBuffer[position + 1]];
        redState = uartDataBuffer[position + 2];
        greenState = uartDataBuffer[position + 3];
        blueState = uartDataBuffer[position + 4];

        //setSwitch(switchId, state);

        setInterfaceLED(ledId, redState, greenState, blueState);

        position += 4;
    }

    apply_update(ALLDRIVERS);
    sendReply[datalen++] = TASK_UART_ACK;
    //sendReply[datalen++] = txnId;
    workingMode = INWALL_WAKE;

    inwallUART_tx(sendReply, datalen);
}

void fanHandler(uint8_t operation)
{
    switch (operation)
    {
    case FAN_OPERATION_INC_LEVEL:
        if (inwallData.fanLevel != FAN_LEVEL_4)
        {
            inwallData.fanLevel++;
            switch (inwallData.fanLevel)
            {
            case 1:
                setInterfaceLED(inwallLEDID[16], FAN_R, FAN_G, FAN_B);
                break;
            case 2:
                setInterfaceLED(inwallLEDID[17], FAN_R, FAN_G, FAN_B);
                break;
            case 3:
                setInterfaceLED(inwallLEDID[18], FAN_R, FAN_G, FAN_B);
                break;
            case 4:
                setInterfaceLED(inwallLEDID[19], FAN_R, FAN_G, FAN_B);
                break;

            default:
                break;
            }
        }
        break;
    case FAN_OPERATION_DEC_LEVEL:
        if (inwallData.fanLevel != FAN_LEVEL_0)
        {
            inwallData.fanLevel--;
            switch (inwallData.fanLevel)
            {
            case 0:
                setInterfaceLED(inwallLEDID[16], 0, 0, 0);
                break;
            case 1:
                setInterfaceLED(inwallLEDID[17], 0, 0, 0);
                break;
            case 2:
                setInterfaceLED(inwallLEDID[18], 0, 0, 0);
                break;
            case 3:
                setInterfaceLED(inwallLEDID[19], 0, 0, 0);
                break;

            default:
                break;
            }
        }
    case FAN_OPERATION_SET_LEDS:
        switch (inwallData.fanLevel)
        {
        case 1:
            setInterfaceLED(inwallLEDID[16], FAN_R, FAN_G, FAN_B);
            break;
        case 2:
            setInterfaceLED(inwallLEDID[16], FAN_R, FAN_G, FAN_B);
            setInterfaceLED(inwallLEDID[17], FAN_R, FAN_G, FAN_B);
            break;
        case 3:
            setInterfaceLED(inwallLEDID[16], FAN_R, FAN_G, FAN_B);
            setInterfaceLED(inwallLEDID[17], FAN_R, FAN_G, FAN_B);
            setInterfaceLED(inwallLEDID[18], FAN_R, FAN_G, FAN_B);
            break;
        case 4:
            setInterfaceLED(inwallLEDID[16], FAN_R, FAN_G, FAN_B);
            setInterfaceLED(inwallLEDID[17], FAN_R, FAN_G, FAN_B);
            setInterfaceLED(inwallLEDID[18], FAN_R, FAN_G, FAN_B);
            setInterfaceLED(inwallLEDID[19], FAN_R, FAN_G, FAN_B);
            break;

        default:
            break;
        }

    default:
        break;
    }
}

void switchPressAction(uint8_t switchID)
{
    inwallSleepCounter = 0;
    if (!workingMode)
    {
        inwallWakeUp();
        //inwallSplash();
        workingMode = INWALL_WAKE;
    }

    //if not splash state save switchpress to run after splash finishes
    switch (switchID)
    {
    case SWITCH_ID1: //scene1
        setInterfaceLED(inwallLEDID[0], SCENE_1_R, SCENE_1_G, SCENE_1_B);
        setInterfaceLED(inwallLEDID[5], SCENE_1_R, SCENE_1_G, SCENE_1_B);
        setInterfaceLED(inwallLEDID[11], SCENE_1_R, SCENE_1_G, SCENE_1_B);

        break;
    case SWITCH_ID2: //scene2
        setInterfaceLED(inwallLEDID[1], SCENE_2_R, SCENE_2_G, SCENE_2_B);
        setInterfaceLED(inwallLEDID[2], SCENE_2_R, SCENE_2_G, SCENE_2_B);
        setInterfaceLED(inwallLEDID[5], SCENE_2_R, SCENE_2_G, SCENE_2_B);
        setInterfaceLED(inwallLEDID[11], SCENE_2_R, SCENE_2_G, SCENE_2_B);
        break;
    case SWITCH_ID3: //scene3
        setInterfaceLED(inwallLEDID[3], SCENE_3_R, SCENE_3_G, SCENE_3_B);
        setInterfaceLED(inwallLEDID[4], SCENE_3_R, SCENE_3_G, SCENE_3_B);
        setInterfaceLED(inwallLEDID[5], SCENE_3_R, SCENE_3_G, SCENE_3_B);
        setInterfaceLED(inwallLEDID[11], SCENE_3_R, SCENE_3_G, SCENE_3_B);
        break;
    case SWITCH_ID4: //decrease scene brightness
        break;
    case SWITCH_ID5: //scene4
        setInterfaceLED(inwallLEDID[6], SCENE_4_R, SCENE_4_G, SCENE_4_B);
        setInterfaceLED(inwallLEDID[5], SCENE_4_R, SCENE_4_G, SCENE_4_B);
        setInterfaceLED(inwallLEDID[11], SCENE_4_R, SCENE_4_G, SCENE_4_B);
        break;
    case SWITCH_ID6: //scene5
        setInterfaceLED(inwallLEDID[7], SCENE_5_R, SCENE_5_G, SCENE_5_B);
        setInterfaceLED(inwallLEDID[8], SCENE_5_R, SCENE_5_G, SCENE_5_B);
        setInterfaceLED(inwallLEDID[5], SCENE_5_R, SCENE_5_G, SCENE_5_B);
        setInterfaceLED(inwallLEDID[11], SCENE_5_R, SCENE_5_G, SCENE_5_B);
        break;
    case SWITCH_ID7: //scene6
        setInterfaceLED(inwallLEDID[9], SCENE_6_R, SCENE_6_G, SCENE_6_B);
        setInterfaceLED(inwallLEDID[10], SCENE_6_R, SCENE_6_G, SCENE_6_B);
        setInterfaceLED(inwallLEDID[5], SCENE_6_R, SCENE_6_G, SCENE_6_B);
        setInterfaceLED(inwallLEDID[11], SCENE_6_R, SCENE_6_G, SCENE_6_B);
        break;
    case SWITCH_ID8: //increase scene brightness
        break;
    case SWITCH_ID9: //switch1
        if (inwallData.arr_switch[0].state)
        {
            inwallData.arr_switch[0].state = 0;
            setInterfaceLED(inwallLEDID[12], GENERAL_WHITE, GENERAL_WHITE, GENERAL_WHITE);
        }
        else
        {
            inwallData.arr_switch[0].state = 0xFF;
            setInterfaceLED(inwallLEDID[12], SWITCH_1_R, SWITCH_1_G, SWITCH_1_B);
        }

        break;
    case SWITCH_ID10: //switch2
        if (inwallData.arr_switch[1].state)
        {
            inwallData.arr_switch[1].state = 0;
            setInterfaceLED(inwallLEDID[13], GENERAL_WHITE, GENERAL_WHITE, GENERAL_WHITE);
        }
        else
        {
            inwallData.arr_switch[1].state = 0xFF;
            setInterfaceLED(inwallLEDID[13], SWITCH_2_R, SWITCH_2_G, SWITCH_2_B);
        }
        break;
    case SWITCH_ID11: //switch3
        if (inwallData.arr_switch[2].state)
        {
            inwallData.arr_switch[2].state = 0;
            setInterfaceLED(inwallLEDID[14], GENERAL_WHITE, GENERAL_WHITE, GENERAL_WHITE);
        }
        else
        {
            inwallData.arr_switch[2].state = 0xFF;
            setInterfaceLED(inwallLEDID[14], SWITCH_3_R, SWITCH_3_G, SWITCH_3_B);
        }
        break;
    case SWITCH_ID12: //fan dec
        fanHandler(FAN_OPERATION_DEC_LEVEL);
        //setInterfaceLED(inwallLEDID[15], 0xFF, 0, 0);
        break;
    case SWITCH_ID13: //switch4
        if (inwallData.arr_switch[3].state)
        {
            inwallData.arr_switch[3].state = 0;
            setInterfaceLED(inwallLEDID[20], GENERAL_WHITE, GENERAL_WHITE, GENERAL_WHITE);
        }
        else
        {
            inwallData.arr_switch[3].state = 0xFF;
            setInterfaceLED(inwallLEDID[20], SWITCH_4_R, SWITCH_4_G, SWITCH_4_B);
        }
        break;
    case SWITCH_ID14: //switch5
        if (inwallData.arr_switch[4].state)
        {
            inwallData.arr_switch[4].state = 0;
            setInterfaceLED(inwallLEDID[21], GENERAL_WHITE, GENERAL_WHITE, GENERAL_WHITE);
        }
        else
        {
            inwallData.arr_switch[4].state = 0xFF;
            setInterfaceLED(inwallLEDID[21], SWITCH_5_R, SWITCH_5_G, SWITCH_5_B);
        }
        break;
    case SWITCH_ID15: //switch6
        if (inwallData.arr_switch[5].state)
        {
            inwallData.arr_switch[5].state = 0;
            setInterfaceLED(inwallLEDID[22], GENERAL_WHITE, GENERAL_WHITE, GENERAL_WHITE);
        }
        else
        {
            inwallData.arr_switch[5].state = 0xFF;
            setInterfaceLED(inwallLEDID[22], SWITCH_6_R, SWITCH_6_G, SWITCH_6_B);
        }
        break;
    case SWITCH_ID16: //fan inc
        fanHandler(FAN_OPERATION_INC_LEVEL);
        //setInterfaceLED(inwallLEDID[23], 0xFF, 0, 0);
        break;
    default:
        break;
    }
    apply_update(ALLDRIVERS);
}