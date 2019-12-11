#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "Mini58Series.h"
#include "keusActions.h"

/**
 * @brief       Mapping values to a different range
 *
 * @param       value to map, min value of input
 *              range, max value of input range, min value of o/p range
 *              max value of o/p range
 *
 * @return      mapped value
 *
 * @details     Required for mapping range 0-255 to 0-100 
 */
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/**
 * @brief       Set pwm duty
 *
 * @param       ledID and required duty cycle
 *
 * @return      Nothing
 *
 * @details     Sets pwm duty cycle for dimming and fan type output
 */
void setPwmDuty(uint8_t ledno, uint8_t duty)
{
    int finalDuty = map(duty, 0, 255, 0, 100);
    finalDuty = 100 - finalDuty; //As board led works in -ve logic
    switch (ledno)               //TODO: replace with cmr to remove redundant calculations
    {
    case LEDID1:
        PWM_ConfigOutputChannel(PWM, KEUS_PWM_CHL0, KEUS_PWM_FREQUENCY, finalDuty);
        break;
    case LEDID2:
        PWM_ConfigOutputChannel(PWM, KEUS_PWM_CHL1, KEUS_PWM_FREQUENCY, finalDuty);
        break;
    case LEDID3:
        PWM_ConfigOutputChannel(PWM, KEUS_PWM_CHL2, KEUS_PWM_FREQUENCY, finalDuty);
        break;
    case LEDID4:
        PWM_ConfigOutputChannel(PWM, KEUS_PWM_CHL3, KEUS_PWM_FREQUENCY, finalDuty);
        break;

    default:
        break;
    }
}

/**
 * @brief       Set PINs as GPIO Output/PWM output
 *
 * @param       Target pin and output type required
 *
 * @return      Nothing
 */
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

/**
 * @brief       Toggle output state on buttonpress
 *
 * @param       ledID
 *
 * @return      Nothing
 *
 * @details     Whenever a button is pressed it toggles the output state and
 *              sends the info via UART
 */
void toggleLed(uint8_t ledno)
{
    uint8_t sendReply[5], datalen = 0;
    sendReply[datalen++] = REPORTSWITCHSTATE;
    updateTxnId();
    sendReply[datalen++] = txnId;

    if (ledno == LEDID1 && arr_led[0].config)
    {

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
        sendReply[datalen++] = LEDID1;
        sendReply[datalen++] = arr_led[0].state;
        UART_tx(sendReply, datalen);
        retryGettingAck(sendReply, datalen);
    }

    else if (ledno == LEDID2 && arr_led[1].config)
    {
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
        sendReply[datalen++] = LEDID2;
        sendReply[datalen++] = arr_led[1].state;
        UART_tx(sendReply, datalen);
        retryGettingAck(sendReply, datalen);
    }

    else if (ledno == LEDID3 && arr_led[2].config)
    {
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
        sendReply[datalen++] = LEDID3;
        sendReply[datalen++] = arr_led[2].state;
        UART_tx(sendReply, datalen);
        retryGettingAck(sendReply, datalen);
    }
    else if (ledno == LEDID4 && arr_led[3].config)
    {
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
        sendReply[datalen++] = LEDID4;
        sendReply[datalen++] = arr_led[3].state;
        UART_tx(sendReply, datalen);
        retryGettingAck(sendReply, datalen);
    }
}

/**
 * @brief       Sets the output as state received from UART
 *
 * @param       Nothing
 *
 * @return      Nothing
 *
 */
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
            if (arr_led[0].config == CONFIGNORMAL)
            {
                if (state > 0)
                {
                    arr_led[0].state = 255;
                    LED1 = LED_HIGH;
                }
                else
                {
                    arr_led[0].state = 0;
                    LED1 = LED_LOW;
                }
            }
            else //for config other than normal: dim & fan
            {
                arr_led[0].state = state;
                setPwmDuty(LEDID1, arr_led[0].state);
            }

            break;
        case LEDID2:
            if (arr_led[1].config == CONFIGNORMAL)
            {
                if (state > 0)
                {
                    arr_led[1].state = 255;
                    LED2 = LED_HIGH;
                }
                else
                {
                    arr_led[1].state = 0;
                    LED2 = LED_LOW;
                }
            }
            else
            {
                arr_led[1].state = state;
                setPwmDuty(LEDID2, arr_led[1].state);
            }

            break;
        case LEDID3:
            if (arr_led[2].config == CONFIGNORMAL)
            {
                if (state > 0)
                {
                    arr_led[2].state = 255;
                    LED3 = LED_HIGH;
                }
                else
                {
                    arr_led[2].state = 0;
                    LED3 = LED_LOW;
                }
            }
            else
            {
                arr_led[2].state = state;
                setPwmDuty(LEDID3, arr_led[2].state);
            }

            break;
        case LEDID4:
            if (arr_led[3].config == 1)
            {
                if (state > 0)
                {
                    arr_led[3].state = 255;
                    LED4 = LED_HIGH;
                }
                else
                {
                    arr_led[3].state = 0;
                    LED4 = LED_LOW;
                }
            }
            else
            {
                arr_led[3].state = state;
                setPwmDuty(LEDID4, arr_led[3].state);
            }
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

/**
 * @brief       Sets timer to send the switch press information message periodically
 *              until ack received 
 *
 * @param       Message, length of the message
 *
 * @return      Nothing
 *
 */
void retryGettingAck(uint8_t arr[], uint8_t dataLen)
{
    for (int i = 0; i < dataLen; i++)
    {
        retryBuffer[i] = arr[i]; //copies to seperate buffer
    }

    retryDataLen = dataLen; // copies to seperate length var

    // Set timer 0 working 1Hz in periodic mode
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1);

    // Enable timer interrupt
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);

    // Start Timer 0
    TIMER_Start(TIMER0);
}


//Sends the switch press info message via uart
void TMR0_IRQHandler(void)
{
    UART_tx(retryBuffer, retryDataLen);

    // clear timer interrupt flag
    TIMER_ClearIntFlag(TIMER0);
}

//copies the uart received data into seperate buffer and call taskHandler to perform task
//Extracts commandID and txnID
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

//Calls the required function as needed by UART received message
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
    case TASK_EXECUTE_SWITCH_STATE:
        executeSwitchState();
        break;

    case TASK_UART_ACK:
        uartAck();
        break;

    default:
        break;
    }
}

//At start up configures all switches as normal off/on type
void keus_config_switch_init(void)
{
    setOutputPins(LEDID1, CONFIGNORMAL);
    setOutputPins(LEDID2, CONFIGNORMAL);
    setOutputPins(LEDID3, CONFIGNORMAL);
    setOutputPins(LEDID4, CONFIGNORMAL);
    arr_led[0].config = CONFIGNORMAL;
    arr_led[1].config = CONFIGNORMAL;
    arr_led[2].config = CONFIGNORMAL;
    arr_led[3].config = CONFIGNORMAL;
}

//Sets output state as on-off/dimming/fan
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

//Sends all switch state info via uart
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

//Sends all switch config info via uart
void getConfig(void)
{
    //Example msg: <replyID><txnID><no of switch><switchid><config><switchid><config>....
    //Send UART: 28<size><commandid 2><txnid>29
    //28 02 03 04 29

    uint8_t sendState[15], position = 3, datalen = 0;
    sendState[datalen++] = GETCONFIGREPLY;
    sendState[datalen++] = txnId;
    sendState[datalen++] = MAX_NUMBER_LED;
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

//Stopes the timer which sends switch press info periodically on getting ack
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