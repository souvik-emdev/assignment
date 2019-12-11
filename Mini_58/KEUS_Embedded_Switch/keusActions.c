#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "Mini58Series.h"
#include "keusActions.h"

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setPwmDuty(uint8_t ledno, uint8_t duty)
{
    int finalDuty = map(duty, 0, 255, 0, 100);
    finalDuty = 100 - finalDuty;
    //printf("duty is: %d", finalDuty);
    switch (ledno)
    {
    case LEDID1:
        PWM_ConfigOutputChannel(PWM, 0, 900, finalDuty);
        break;
    case LEDID2:
        PWM_ConfigOutputChannel(PWM, 1, 900, finalDuty);
        break;
    case LEDID3:
        PWM_ConfigOutputChannel(PWM, 2, 900, finalDuty);
        break;
    case LEDID4:
        PWM_ConfigOutputChannel(PWM, 3, 900, finalDuty);
        break;

    default:
        break;
    }
}

void setOutputPins(uint8_t pin, uint8_t config)
{
    switch (pin)
    {
    case LEDID1: //p2.2
        if (config == CONFIGNORMAL)
        {
            SYS->P2_MFP &= ~SYS_MFP_P22_Msk;
            SYS->P2_MFP |= SYS_MFP_P22_GPIO;
            GPIO_SetMode(P2, BIT2, GPIO_MODE_OUTPUT);
            P22 = LED_LOW;
        }
        else
        {
            SYS->P2_MFP &= ~SYS_MFP_P22_Msk;
            SYS->P2_MFP |= SYS_MFP_P22_PWM0_CH0;
        }
        break;

    case LEDID2: //p2.3
        if (config == CONFIGNORMAL)
        {
            SYS->P2_MFP &= ~SYS_MFP_P23_Msk;
            SYS->P2_MFP |= SYS_MFP_P23_GPIO;
            GPIO_SetMode(P2, BIT3, GPIO_MODE_OUTPUT);
            P23 = LED_LOW;
        }
        else
        {
            SYS->P2_MFP &= ~SYS_MFP_P23_Msk;
            SYS->P2_MFP |= SYS_MFP_P23_PWM0_CH1;
        }
        break;

    case LEDID3: //p2.4
        if (config == CONFIGNORMAL)
        {
            SYS->P2_MFP &= ~SYS_MFP_P24_Msk;
            SYS->P2_MFP |= SYS_MFP_P24_GPIO;
            GPIO_SetMode(P2, BIT4, GPIO_MODE_OUTPUT);
            P24 = LED_LOW;
        }
        else
        {
            SYS->P2_MFP &= ~SYS_MFP_P24_Msk;
            SYS->P2_MFP |= SYS_MFP_P24_PWM0_CH2;
        }
        break;

    case LEDID4: //p2.5
        if (config == CONFIGNORMAL)
        {
            SYS->P2_MFP &= ~SYS_MFP_P25_Msk;
            SYS->P2_MFP |= SYS_MFP_P25_GPIO;
            GPIO_SetMode(P2, BIT5, GPIO_MODE_OUTPUT);
            P25 = LED_LOW;
        }
        else
        {
            SYS->P2_MFP &= ~SYS_MFP_P25_Msk;
            SYS->P2_MFP |= SYS_MFP_P25_PWM0_CH3;
        }
        break;
    default:
        break;
    }
}

void toggleLed(uint8_t ledno)
{
    uint8_t sendReply[5], datalen = 0;
    sendReply[datalen++] = REPORTSWITCHSTATE;
    updateTxnId();
    sendReply[datalen++] = txnId;

    if (ledno == LEDID1 && arr_led[0].config)
    {

        toggleLedTask(LEDID1);
        sendReply[datalen++] = LEDID1;
        sendReply[datalen++] = arr_led[0].state;
        UART_tx(sendReply, datalen);
        retryGettingAck(sendReply, datalen);
    }

    if (ledno == LEDID2 && arr_led[1].config)
    {
        toggleLedTask(LEDID2);
        sendReply[datalen++] = LEDID2;
        sendReply[datalen++] = arr_led[1].state;
        UART_tx(sendReply, datalen);
        retryGettingAck(sendReply, datalen);
    }

    if (ledno == LEDID3 && arr_led[2].config)
    {
        toggleLedTask(LEDID3);
        sendReply[datalen++] = LEDID3;
        sendReply[datalen++] = arr_led[2].state;
        UART_tx(sendReply, datalen);
        retryGettingAck(sendReply, datalen);
    }
    if (ledno == LEDID4 && arr_led[3].config)
    {
        toggleLedTask(LEDID4);
        sendReply[datalen++] = LEDID4;
        sendReply[datalen++] = arr_led[3].state;
        UART_tx(sendReply, datalen);
        retryGettingAck(sendReply, datalen);
    }
    // UART_tx(sendReply, datalen);
    // retryGettingAck(sendReply, datalen);
}

//ak: implement in toggleLed
void toggleLedTask(uint8_t ledID)
{
    switch (ledID)
    {
    case LEDID1:
        if (arr_led[0].state < 1)
        {
            arr_led[0].state = 255;
            if (arr_led[0].config == 1)
            {
                LED1 = LED_HIGH;
            }
            else
            {
                setPwmDuty(LEDID1, arr_led[0].state);
            }
        }
        else
        {
            arr_led[0].state = 0;
            if (arr_led[0].config == 1)
            {
                LED1 = LED_LOW;
            }
            else
            {
                setPwmDuty(LEDID1, arr_led[0].state);
            }
        }
        break;
    case LEDID2:
        if (arr_led[1].state < 1)
        {
            arr_led[1].state = 255;
            if (arr_led[1].config == 1)
            {
                LED2 = LED_HIGH;
            }
            else
            {
                setPwmDuty(LEDID2, arr_led[1].state);
            }
        }
        else
        {
            arr_led[1].state = 0;
            if (arr_led[1].config == 1)
            {
                LED2 = LED_LOW;
            }
            else
            {
                setPwmDuty(LEDID2, arr_led[1].state);
            }
        }
        break;
    case LEDID3:
        if (arr_led[2].state < 1)
        {
            arr_led[2].state = 255;
            if (arr_led[2].config == 1)
            {
                LED3 = LED_HIGH;
            }
            else
            {
                setPwmDuty(LEDID3, arr_led[2].state);
            }
        }
        else
        {
            arr_led[2].state = 0;
            if (arr_led[2].config == 1)
            {
                LED3 = LED_LOW;
            }
            else
            {
                setPwmDuty(LEDID3, arr_led[2].state);
            }
        }
        break;
    case LEDID4:
        if (arr_led[3].state < 1)
        {
            arr_led[3].state = 255;
            if (arr_led[3].config == 1)
            {
                LED4 = LED_HIGH;
            }
            else
            {
                setPwmDuty(LEDID4, arr_led[3].state);
            }
        }
        else
        {
            arr_led[3].state = 0;
            if (arr_led[3].config == 1)
            {
                LED4 = LED_LOW;
            }
            else
            {
                setPwmDuty(LEDID4, arr_led[3].state);
            }
        }
        break;

    default:
        break;
    }
}

void executeSwitchState(void)
{
    //executeSwitchState: 28<size><commandid 4><txn><noof switch><id><state>..<29>
    //<noof switch><id><state><id><state>
    int noOfSwitch = uartDataBuffer[0];
    uint8_t position = 0, switchId, state, sendReply[5], datalen = 0;

    for (int i = 0; i < noOfSwitch; i++)
    {
        switchId = uartDataBuffer[position + 1];
        state = uartDataBuffer[position + 2];

        switch (switchId)
        {
        case LEDID1:
            if (arr_led[0].config == 1)
            {
                if (state > 1)  //ak: state > 0
                    arr_led[0].state = 255;
                else
                    arr_led[0].state = 0;
            }
            else
                arr_led[0].state = state;
            implementSwitchState(LEDID1);
            break;
        case LEDID2:
            if (arr_led[1].config == 1)
            {
                if (state > 1)
                    arr_led[1].state = 255;
                else
                    arr_led[1].state = 0;
            }
            else
                arr_led[1].state = state;
            implementSwitchState(LEDID2);
            break;
        case LEDID3:
            if (arr_led[2].config == 1)
            {
                if (state > 1)
                    arr_led[2].state = 255;
                else
                    arr_led[2].state = 0;
            }
            else
                arr_led[2].state = state;
            implementSwitchState(LEDID3);
            break;
        case LEDID4:
            if (arr_led[3].config == 1)
            {
                if (state > 1)
                    arr_led[3].state = 255;
                else
                    arr_led[3].state = 0;
            }
            else
                arr_led[3].state = state;
            implementSwitchState(LEDID4);
            break;
        default:
            break;
        }

        position += 2;
    }
    sendReply[datalen++] = TASK_UART_ACK;
    sendReply[datalen++] = txnId;

    UART_tx(sendReply, datalen);
}

//ak: try to implement this in execute switch state
void implementSwitchState(uint8_t ledno)
{
    switch (ledno)
    {
    case LEDID1:
        if (arr_led[0].config == 1)
        {
            if (arr_led[0].state > 0)
            {
                LED1 = LED_HIGH;
            }
            else
            {
                LED1 = LED_LOW;
            }
        }
        else
        {
            setPwmDuty(LEDID1, arr_led[0].state);
        }
        break;
    case LEDID2:
        if (arr_led[1].config == 1)
        {
            if (arr_led[1].state > 0)
            {
                LED2 = LED_HIGH;
            }
            else
            {
                LED2 = LED_LOW;
            }
        }
        else
        {
            setPwmDuty(LEDID2, arr_led[1].state);
        }
        break;
    case LEDID3:
        if (arr_led[2].config == 1)
        {
            if (arr_led[2].state > 0)
            {
                LED3 = LED_HIGH;
            }
            else
            {
                LED3 = LED_LOW;
            }
        }
        else
        {
            setPwmDuty(LEDID3, arr_led[2].state);
        }
        break;
    case LEDID4:
        if (arr_led[3].config == 1)
        {
            if (arr_led[3].state > 0)
            {
                LED4 = LED_HIGH;
            }
            else
            {
                LED4 = LED_LOW;
            }
        }
        else
        {
            setPwmDuty(LEDID4, arr_led[3].state);
        }
        break;

    default:
        break;
    }
}

void retryGettingAck(uint8_t arr[], uint8_t dataLen)
{
    for (int i = 0; i < dataLen; i++)
    {
        retryBuffer[i] = arr[i];
    }

    retryDataLen = dataLen;

    // Set timer 0 working 1Hz in periodic mode
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1);

    // Enable timer interrupt
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);

    // Start Timer 0
    TIMER_Start(TIMER0);
}

void TMR0_IRQHandler(void)
{
    UART_tx(retryBuffer, retryDataLen);

    // clear timer interrupt flag
    TIMER_ClearIntFlag(TIMER0);
}

void parseUart(void)
{
    int i = 0, j = 2;
    commandId = g0_u8RecData[0];
    txnId = g0_u8RecData[1];
    for (i = 0; i < MAX_UART_DATA_BUFFER; i++)
    {
        uartDataBuffer[i] = g0_u8RecData[j];
        j++;
    }

    taskHandler();
}

void taskHandler(void)
{
    switch (commandId)
    {
    case TASK_CONFIG_SWITCH:
        configSwitch();
        break;
    case TASK_GET_SWITCH_STATE:
        getSwitchState();
        break;

    case TASK_GET_CONFIG:
        getConfig();
        break;
    case TASK_LED_CONTROL:
        executeSwitchState();
        break;

    case TASK_UART_ACK:
        uartAck();
        break;

    default:
        break;
    }
}

void configSwitch(void)
{
    //Example msg: <no of switch><switchid><config><switchid><config>....
    //Send UART:
    //configSwitch: 28<size><commandid 1><txnid><no of switch><switchid><config><switchid><config>29

    int noOfSwitch = uartDataBuffer[0];
    uint8_t position = 0, switchId, config, sendReply[5], datalen = 0;

    for (int i = 0; i < noOfSwitch; i++)
    {
        switchId = uartDataBuffer[position + 1];
        config = uartDataBuffer[position + 2];

        switch (switchId)
        {
        case LEDID1:
            arr_led[0].config = config;
            setOutputPins(LEDID1, config);
            break;
        case LEDID2:
            arr_led[1].config = config;
            setOutputPins(LEDID2, config);
            break;
        case LEDID3:
            arr_led[2].config = config;
            setOutputPins(LEDID3, config);
            break;
        case LEDID4:
            arr_led[3].config = config;
            setOutputPins(LEDID4, config);
            break;
        default:
            break;
        }

        position += 2;
    }
    sendReply[datalen++] = TASK_UART_ACK;
    sendReply[datalen++] = txnId;

    UART_tx(sendReply, datalen);
}

void getSwitchState(void)
{
    //Example msg: <replyID><txnID><no of switch><switchid><state><switchid><state>....
    //Send UART: 28<size><commandid 2><txnid>29
    //28 02 02 04 29

    uint8_t sendState[15], position = 3, datalen = 0;
    sendState[0] = GETSWITCHSTATEREPLY;
    datalen++;
    sendState[1] = txnId;
    datalen++;
    sendState[2] = MAX_NUMBER_LED;
    datalen++;
    for (int i = 0; i < MAX_NUMBER_LED; i++)
    {
        sendState[position] = i + 1; //switch ID
        datalen++;
        sendState[position + 1] = arr_led[i].state;
        datalen++;
        position += 2;
    }

    UART_tx(sendState, datalen);
}

void getConfig(void)
{
    //Example msg: <replyID><txnID><no of switch><switchid><config><switchid><config>....
    //Send UART: 28<size><commandid 2><txnid>29
    //28 02 03 04 29

    uint8_t sendState[15], position = 3, datalen = 0;
    //ak: use datalen for array index 
    sendState[0] = GETCONFIGREPLY;
    datalen++;
    sendState[1] = txnId;
    datalen++;
    sendState[2] = MAX_NUMBER_LED;
    datalen++;
    for (int i = 0; i < MAX_NUMBER_LED; i++)
    {
        sendState[position] = i + 1; //switch ID
        datalen++;
        sendState[position + 1] = arr_led[i].config;
        datalen++;
        position += 2;
    }

    UART_tx(sendState, datalen);
}

void uartAck(void)
{
    if (txnId == retryBuffer[1])
    {
        TIMER_Close(TIMER0);
    }
}

void updateTxnId(void)
{
    if (txnId == 255)
        txnId = 0;
    txnId++;
}