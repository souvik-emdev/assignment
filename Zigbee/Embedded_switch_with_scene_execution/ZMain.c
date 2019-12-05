/**************************************************************************************************
  Filename:       ZMain.c
  Revised:        $Date: 2019-12-05 16:25:30 -0700 (Thrusday, 05 Dec 2019) $

  Description:    Storing and executing KEUS Scene, Switch press action, Controlling GPIOs through UART.
                  Data transmission format: Startbyte-datalength-data-endbyte
                  Switch press should toggle corresponding LED
                  UART Commands to set single/group led state, save scene, execute scene, delete scene
                  & single blink, wait and blink, continuous blink.
                  LED Control:
                  00 - off
                  01 - on
                  02 - if previously off turnon<wait>turnoff
                       if previously on turnoff<wait>turnon<wait>turnoff
                  03 - Blink continuously
                  All scene data is being saved to nvic memory and should retain after power cycle
  Notes:          This version targets the Chipcon CC2530



**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#ifndef NONWK
#include "AF.h"
#endif
#include "hal_adc.h"
#include "hal_flash.h"
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_uart.h"
#include "hal_drivers.h"
#include "OnBoard.h"
#include "OSAL.h"
#include "OSAL_Nv.h"
#include "ZComDef.h"
#include "ZMAC.h"
#include "keus_gpio_util.h"
#include "keus_timer_util.h"

/*********************************************************************
 * CUSTOM CODE
 */
#define KEUS_UART_MSG_INITIATOR 0x28
#define KEUS_UART_MSG_TERMINATOR 0x29
#define KEUS_UART_BUFFER 32
#define KEUS_UART_MSG_ACK 0x09

//Board LED works on Negative Logic
#define LED_ON 0x00
#define LED_OFF 0x01

//Error codes to return
#define INVALID_DATA 0XFF
#define CONTROL_INVALID_STATE 0X1F
#define CONTROL_INVALID_LED 0X2F
#define SAVE_OVERWRITE 0X3F
#define SAVE_MEMORY_FULL 0X4F
#define SAVE_INVALID_LED 0X5F
#define EXECUTE_ID_NOT_FOUND 0X6F
#define DELETE_ID_NOT_FOUND 0X7F

#define MAX_SCENE_STORAGE 10
#define SCN_ARR_NVIC_LOCATION 0x10

//This will set when data received in valid format. In loop state of this is monitored
uint8 message_received = 0;

//Communication variables
uint8 g0_u8RecData[KEUS_UART_BUFFER] = {0};
uint8 g0_bufferCounter = 0;
uint8 g0_shouldSaveToBuffer = 0;
uint8 g0_endMessageIndex = 0;

//GPIO Info
KeusGPIOPin ledPin = {0, 0, GPIO_OUTPUT, false, LED_OFF};
KeusGPIOPin ledPin2 = {0, 1, GPIO_OUTPUT, false, LED_OFF};
KeusGPIOPin ledPin3 = {0, 4, GPIO_OUTPUT, false, LED_ON}; //Hal toggles this at startup
KeusGPIOPin ledPin4 = {0, 5, GPIO_OUTPUT, false, LED_OFF};
KeusGPIOPin buttonPin = {1, 2, GPIO_INPUT, true, LED_ON};
KeusGPIOPin buttonPin2 = {1, 3, GPIO_INPUT, true, LED_ON};
KeusGPIOPin buttonPin3 = {1, 4, GPIO_INPUT, true, LED_ON};
KeusGPIOPin buttonPin4 = {1, 5, GPIO_INPUT, true, LED_ON};

void initUart(void);
void uartRxCb(uint8 port, uint8 event);
void KEUS_delayms(uint16 ms);
void KEUS_init(void);
void KEUS_start(void);
void KEUS_loop(void); //the actual polling
void debounceTimerCbk(uint8 timerId); //debounce timer for switch press

//Timers for ledcontrol mode 2 and 3. Each timer controls single led
void led1BlinkTimerCbk(uint8 timerId);
void led2BlinkTimerCbk(uint8 timerId);
void led3BlinkTimerCbk(uint8 timerId);
void led4BlinkTimerCbk(uint8 timerId);

void ledControl(void);
void saveScene(void);
void executeScene(void);
void deleteScene(void);
void groupControl(void);
void KeusUartAckMsg(uint8); //sends ack and error msg reply

//does nvic operations and returns status of nvic operations
// bool KeusThemeSwitchMiniMemoryInit(void);
// bool KeusThemeSwitchMiniWriteConfigDataIntoMemory(void);
// bool KeusThemeSwitchMiniReadConfigDataIntoMemory(void);

volatile bool debounce_read = true; //only read switchPress if this set

//status of nvic operations
// uint8 initStatus = 0;
// uint8 writeStatus = 0;
// uint8 readStatus = 0;

//controls timings of ledcontrol mode 2 and 3. Does ++ at timercallback
uint8 led1BlinkTimerCounter = 0;
uint8 led2BlinkTimerCounter = 0;
uint8 led3BlinkTimerCounter = 0;
uint8 led4BlinkTimerCounter = 0;

/*sets states of led blinking. Used in ledcontrol mode 2 and 3
each array element for one led
Example: blinkState[0] = 0; led1 not in blink mode
blinkState[0] = 1; led1 is in ledcontrol mode 2, previous state was off
blinkState[0] = 2 & 3; led1 is in ledcontrol mode 2, previous state was on
blinkState[0] = 4 & 5; led1 is in ledcontrol mode 3, continuous blink
*/
uint8 blinkState[4];

struct scene
{
  uint8 id;
  uint8 ledPin;
  uint8 ledPin2;
  uint8 ledPin3;
  uint8 ledPin4;
  uint8 included[4]; //for differentiating which leds are part of the scene. Each element represents one led
};

struct scene arr_scene[MAX_SCENE_STORAGE]; //array of struct of scenes

KeusTimerConfig debounceTimer = {
    &debounceTimerCbk,
    200,
    true,
    -1,
    0};

void debounceTimerCbk(uint8 timerId)
{
  debounce_read = true;
  KeusTimerUtilRemoveTimer(&debounceTimer);
}

KeusTimerConfig led1Timer = {
    &led1BlinkTimerCbk,
    200,
    true,
    -1,
    0};

void led1BlinkTimerCbk(uint8 timerId)
{
  if (blinkState[0] == 1) //mode 2, previous off
  {
    ledPin.state = LED_OFF;
    KeusGPIOSetPinValue(&ledPin);
    blinkState[0] = 0;
    KeusTimerUtilRemoveTimer(&led1Timer);
  }
  else if (blinkState[0] == 2) //mode 2, previous on, turn off time
  {
    ledPin.state = LED_ON;
    KeusGPIOSetPinValue(&ledPin);
    blinkState[0] = 3;
  }
  else if (blinkState[0] == 3) //mode 2, previous on, turn on time
  {
    ledPin.state = LED_OFF;
    KeusGPIOSetPinValue(&ledPin);
    blinkState[0] = 0;
    KeusTimerUtilRemoveTimer(&led1Timer);
  }
  else if (blinkState[0] == 4) //mode 3, turn on time
  {
    ledPin.state = LED_OFF;
    KeusGPIOSetPinValue(&ledPin);
    blinkState[0] = 5;
  }

  else if (blinkState[0] == 5) //mode 3, turn off time
  {
    led1BlinkTimerCounter++;
    if (led1BlinkTimerCounter >= 5)
    {
      ledPin.state = LED_ON;
      KeusGPIOSetPinValue(&ledPin);
      blinkState[0] = 4;
      led1BlinkTimerCounter = 0;
    }
  }
  else if (blinkState[0] == 0) //blink clear
  {
    KeusTimerUtilRemoveTimer(&led1Timer);
  }
}

KeusTimerConfig led2Timer = {
    &led2BlinkTimerCbk,
    100,
    true,
    -1,
    0};

void led2BlinkTimerCbk(uint8 timerId)
{
  if (blinkState[1] == 1)
  {
    ledPin2.state = LED_OFF;
    KeusGPIOSetPinValue(&ledPin2);
    blinkState[1] = 0;
    KeusTimerUtilRemoveTimer(&led2Timer);
  }
  else if (blinkState[1] == 2)
  {
    ledPin2.state = LED_ON;
    KeusGPIOSetPinValue(&ledPin2);
    blinkState[1] = 3;
  }
  else if (blinkState[1] == 3)
  {
    ledPin2.state = LED_OFF;
    KeusGPIOSetPinValue(&ledPin2);
    blinkState[1] = 0;
    KeusTimerUtilRemoveTimer(&led2Timer);
  }
  else if (blinkState[1] == 4)
  {
    ledPin2.state = LED_OFF;
    KeusGPIOSetPinValue(&ledPin2);
    blinkState[1] = 5;
  }

  else if (blinkState[1] == 5)
  {
    led2BlinkTimerCounter++;
    if (led2BlinkTimerCounter >= 5)
    {
      ledPin2.state = LED_ON;
      KeusGPIOSetPinValue(&ledPin2);
      blinkState[1] = 4;
      led2BlinkTimerCounter = 0;
    }
  }
  else if (blinkState[1] == 0)
  {
    KeusTimerUtilRemoveTimer(&led2Timer);
  }
}

KeusTimerConfig led3Timer = {
    &led3BlinkTimerCbk,
    100,
    true,
    -1,
    0};

void led3BlinkTimerCbk(uint8 timerId)
{
  if (blinkState[2] == 1)
  {
    ledPin3.state = LED_OFF;
    KeusGPIOSetPinValue(&ledPin3);
    blinkState[2] = 0;
    KeusTimerUtilRemoveTimer(&led3Timer);
  }
  else if (blinkState[2] == 2)
  {
    ledPin3.state = LED_ON;
    KeusGPIOSetPinValue(&ledPin3);
    blinkState[2] = 3;
  }
  else if (blinkState[2] == 3)
  {
    ledPin3.state = LED_OFF;
    KeusGPIOSetPinValue(&ledPin3);
    blinkState[2] = 0;
    KeusTimerUtilRemoveTimer(&led3Timer);
  }
  else if (blinkState[2] == 4)
  {
    ledPin3.state = LED_OFF;
    KeusGPIOSetPinValue(&ledPin3);
    blinkState[2] = 5;
  }

  else if (blinkState[2] == 5)
  {
    led3BlinkTimerCounter++;
    if (led3BlinkTimerCounter >= 5)
    {
      ledPin3.state = LED_ON;
      KeusGPIOSetPinValue(&ledPin3);
      blinkState[2] = 4;
      led3BlinkTimerCounter = 0;
    }
  }
  else if (blinkState[2] == 0)
  {
    KeusTimerUtilRemoveTimer(&led3Timer);
  }
}

KeusTimerConfig led4Timer = {
    &led4BlinkTimerCbk,
    100,
    true,
    -1,
    0};

void led4BlinkTimerCbk(uint8 timerId)
{
  if (blinkState[3] == 1)
  {
    ledPin4.state = LED_OFF;
    KeusGPIOSetPinValue(&ledPin4);
    blinkState[3] = 0;
    KeusTimerUtilRemoveTimer(&led4Timer);
  }
  else if (blinkState[3] == 2)
  {
    ledPin4.state = LED_ON;
    KeusGPIOSetPinValue(&ledPin4);
    blinkState[3] = 3;
  }
  else if (blinkState[3] == 3)
  {
    ledPin4.state = LED_OFF;
    KeusGPIOSetPinValue(&ledPin4);
    blinkState[3] = 0;
    KeusTimerUtilRemoveTimer(&led4Timer);
  }
  else if (blinkState[3] == 4)
  {
    ledPin4.state = LED_OFF;
    KeusGPIOSetPinValue(&ledPin4);
    blinkState[3] = 5;
  }

  else if (blinkState[3] == 5)
  {
    led4BlinkTimerCounter++;
    if (led4BlinkTimerCounter >= 5)
    {
      ledPin4.state = LED_ON;
      KeusGPIOSetPinValue(&ledPin4);
      blinkState[3] = 4;
      led4BlinkTimerCounter = 0;
    }
  }
  else if (blinkState[3] == 0)
  {
    KeusTimerUtilRemoveTimer(&led4Timer);
  }
}

void initUart()
{
  halUARTCfg_t uartConfig;
  uartConfig.configured = TRUE;
  uartConfig.baudRate = HAL_UART_BR_115200;
  uartConfig.flowControl = FALSE;
  uartConfig.flowControlThreshold = 48;
  uartConfig.rx.maxBufSize = 128;
  uartConfig.tx.maxBufSize = 128;
  uartConfig.idleTimeout = 6;
  uartConfig.intEnable = TRUE;
  uartConfig.callBackFunc = uartRxCb;
  HalUARTOpen(HAL_UART_PORT_0, &uartConfig);
}

void uartRxCb(uint8 port, uint8 event)
{ 
  //Example syntax: 28 03 01 03 01 29
  uint8 u8InChar;

  while (Hal_UART_RxBufLen(port))
  {
    // Read one byte from UART to ch
    HalUARTRead(port, &u8InChar, 1);

    // if (u8InChar == 's')
    // {
    //   ledPin.state = LED_ON;
    //   KeusGPIOSetPinValue(&ledPin);
    // }
    // else if (u8InChar == 'r')
    // {
    //   ledPin.state = LED_OFF;
    //   KeusGPIOSetPinValue(&ledPin);
    // }

    if (u8InChar == KEUS_UART_MSG_INITIATOR && g0_endMessageIndex == 0)
    {
      g0_shouldSaveToBuffer = 1;
      g0_bufferCounter = 0;
    }
    else if (g0_endMessageIndex == 0 && g0_shouldSaveToBuffer)
    {
      g0_endMessageIndex = u8InChar;
    }
    else if (u8InChar == KEUS_UART_MSG_TERMINATOR && g0_endMessageIndex == g0_bufferCounter)
    {
      g0_shouldSaveToBuffer = 0;
      g0_endMessageIndex = 0;

      message_received = 1; //flag to process data
    }
    else if (g0_bufferCounter > g0_endMessageIndex)
    {
      g0_shouldSaveToBuffer = 0;
      g0_endMessageIndex = 0;
    }

    else if (g0_shouldSaveToBuffer)
    {
      g0_u8RecData[g0_bufferCounter++] = u8InChar;
    }
    else
      g0_endMessageIndex = 0;
  }
}

void KEUS_delayms(uint16 ms)
{
  for (uint16 i = 0; i < ms; i++)
  {
    Onboard_wait(1000);
  }
}

void KEUS_init()
{
  initUart();
  KeusGPIOSetDirection(&ledPin);
  KeusGPIOSetDirection(&ledPin2);
  KeusGPIOSetDirection(&ledPin3);
  KeusGPIOSetDirection(&ledPin4);
  KeusGPIOSetDirection(&buttonPin);
  KeusGPIOSetDirection(&buttonPin2);
  KeusGPIOSetDirection(&buttonPin3);
  KeusGPIOSetDirection(&buttonPin4);
  KeusGPIOSetPinValue(&ledPin);
  KeusGPIOSetPinValue(&ledPin2);
  KeusGPIOSetPinValue(&ledPin3);
  KeusGPIOSetPinValue(&ledPin4);
  KeusGPIOEdgeConfiguration(&buttonPin, GPIO_RISING_EDGE);
  KeusGPIOEdgeConfiguration(&buttonPin2, GPIO_RISING_EDGE);
  KeusGPIOEdgeConfiguration(&buttonPin3, GPIO_RISING_EDGE);
  KeusGPIOEdgeConfiguration(&buttonPin4, GPIO_RISING_EDGE);
  KeusGPIOInterruptEnable(&buttonPin);
  KeusGPIOInterruptEnable(&buttonPin2);
  KeusGPIOInterruptEnable(&buttonPin3);
  KeusGPIOInterruptEnable(&buttonPin4);
  KeusTimerUtilInit();
  KeusTimerUtilStartTimer();
  //initStatus = KeusThemeSwitchMiniMemoryInit();
  osal_nv_item_init(SCN_ARR_NVIC_LOCATION, sizeof(arr_scene), (void *)arr_scene);
  //readStatus = KeusThemeSwitchMiniReadConfigDataIntoMemory();
  //restoring scenes back to memory at startup
  osal_nv_read(SCN_ARR_NVIC_LOCATION, 0, sizeof(arr_scene), (void *)arr_scene);
  HalUARTWrite(HAL_UART_PORT_0, "KEUS INIT", (byte)osal_strlen("KEUS INIT"));
}

// bool KeusThemeSwitchMiniMemoryInit(void)
// {

//   uint8 res = osal_nv_item_init(0x10, sizeof(arr_scene), (void *)arr_scene);

//   if (res == SUCCESS || res == NV_ITEM_UNINIT)
//   {
//     return true;
//   }
//   else
//   {
//     return false;
//   }
// }

// bool KeusThemeSwitchMiniWriteConfigDataIntoMemory(void)
// {
//   uint8 res = osal_nv_write(0x10, 0, sizeof(arr_scene), (void *)arr_scene);

//   if (res == SUCCESS)
//   {
//     return true;
//   }
//   else
//   {
//     return false;
//   }
// }

// bool KeusThemeSwitchMiniReadConfigDataIntoMemory(void)
// {
//   uint8 res = osal_nv_read(0x10, 0, sizeof(arr_scene), (void *)arr_scene);

//   if (res == SUCCESS)
//   {
//     return true;
//   }
//   else
//   {
//     return false;
//   }
// }

void KEUS_loop()
{
  while (1)
  {
    HalUARTPoll();
    //KEUS_delayms(1000);
    if (message_received) //uart flag
    {
      //Example Msg array: 01 03 01
      switch (g0_u8RecData[0]) //position of command in array
      {
      case 1:
        ledControl();
        break;
      case 2:
        saveScene();
        break;
      case 3:
        executeScene();
        break;
      case 4:
        deleteScene();
        break;
      case 5:
        groupControl();
        break;
      default:
        KeusUartAckMsg(INVALID_DATA);
        break;
      }
      message_received = 0; //clear uart flag
    }
  }
}

void groupControl(void)
{
  //Example array: 05 02 00 01 01 00 | Command,No of leds,Ledno,ledstate.....
  int i = 0;
  uint8 ledPointer = 2; //position of ledno in uart data array
  int loopLength = g0_u8RecData[1]; //position of no of leds in uart array
  for (i = 0; i < loopLength; i++)
  {
    switch (g0_u8RecData[ledPointer]) //led position; Each case for one led
    {
    case 0:
      ledPin.state = !(g0_u8RecData[ledPointer + 1]); //Led state position in uart array. ! for -ve logic
      KeusGPIOSetPinValue(&ledPin);
      blinkState[0] = 0;
      break;
    case 1:
      ledPin2.state = !(g0_u8RecData[ledPointer + 1]);
      KeusGPIOSetPinValue(&ledPin2);
      blinkState[1] = 0;
      break;
    case 2:
      ledPin3.state = !(g0_u8RecData[ledPointer + 1]);
      KeusGPIOSetPinValue(&ledPin3);
      blinkState[2] = 0;
      break;
    case 3:
      ledPin4.state = !(g0_u8RecData[ledPointer + 1]);
      KeusGPIOSetPinValue(&ledPin4);
      blinkState[3] = 0;
      break;
    }
    ledPointer += 2; //next led position
  }
}

void ledControl(void)
{
  //Example msg : 01 03 01
  uint8 ledState = 0;
  uint8 ledNo = g0_u8RecData[1]; //position of led in uart array

  if ((g0_u8RecData[2] == 1) || (g0_u8RecData[2] == 0)) //for 1 and 0, change state
  {

    if (g0_u8RecData[2])
      ledState = LED_ON;
    else
      ledState = LED_OFF;
    switch (ledNo)
    {
    case 0:
      ledPin.state = ledState;
      blinkState[0] = 0;
      KeusGPIOSetPinValue(&ledPin);
      KeusUartAckMsg(KEUS_UART_MSG_ACK);
      break;

    case 1:
      ledPin2.state = ledState;
      blinkState[1] = 0;
      KeusGPIOSetPinValue(&ledPin2);
      KeusUartAckMsg(KEUS_UART_MSG_ACK);
      break;

    case 2:
      ledPin3.state = ledState;
      blinkState[2] = 0;
      KeusGPIOSetPinValue(&ledPin3);
      KeusUartAckMsg(KEUS_UART_MSG_ACK);
      break;

    case 3:
      ledPin4.state = ledState;
      blinkState[3] = 0;
      KeusGPIOSetPinValue(&ledPin4);
      KeusUartAckMsg(KEUS_UART_MSG_ACK);
      break;
    default:
      KeusUartAckMsg(CONTROL_INVALID_LED);
      break;
    }
  }
  else if (g0_u8RecData[2] == 2)
  {
    //do blink once
    switch (ledNo)
    {
    case 0:
      if (ledPin.state == LED_OFF) //check the previous state if it was off
      { 
        ledPin.state = LED_ON;
        KeusGPIOSetPinValue(&ledPin);
        blinkState[0] = 1;
        KeusTimerUtilAddTimer(&led1Timer);
      }
      else //check the previous state if it was on
      {
        ledPin.state = LED_OFF;
        KeusGPIOSetPinValue(&ledPin);
        blinkState[0] = 2;
        KeusTimerUtilAddTimer(&led1Timer);
      }

      break;

    case 1:
      if (ledPin2.state == LED_OFF)
      {
        ledPin2.state = LED_ON;
        KeusGPIOSetPinValue(&ledPin2);
        blinkState[1] = 1;
        KeusTimerUtilAddTimer(&led2Timer);
      }
      else
      {
        ledPin2.state = LED_OFF;
        KeusGPIOSetPinValue(&ledPin2);
        blinkState[1] = 2;
        KeusTimerUtilAddTimer(&led2Timer);
      }
      break;
    case 2:
      if (ledPin3.state == LED_OFF)
      { //off
        ledPin3.state = LED_ON;
        KeusGPIOSetPinValue(&ledPin3);
        blinkState[2] = 1;
        KeusTimerUtilAddTimer(&led3Timer);
      }
      else
      {
        ledPin3.state = LED_OFF;
        KeusGPIOSetPinValue(&ledPin3);
        blinkState[2] = 2;
        KeusTimerUtilAddTimer(&led3Timer);
      }
      break;

    case 3:
      if (ledPin4.state == LED_OFF)
      { //off
        ledPin4.state = LED_ON;
        KeusGPIOSetPinValue(&ledPin4);
        blinkState[3] = 1;
        KeusTimerUtilAddTimer(&led4Timer);
      }
      else
      {
        ledPin4.state = LED_OFF;
        KeusGPIOSetPinValue(&ledPin4);
        blinkState[3] = 2;
        KeusTimerUtilAddTimer(&led4Timer);
      }
      break;
    }
  }
  else if (g0_u8RecData[2] == 3)
  {
    //do continuous blinking
    switch (ledNo)
    {
    case 0:
      ledPin.state = LED_ON;
      KeusGPIOSetPinValue(&ledPin);
      blinkState[0] = 4;
      KeusTimerUtilAddTimer(&led1Timer);
      break;

    case 1:
      ledPin2.state = LED_ON;
      KeusGPIOSetPinValue(&ledPin2);
      blinkState[1] = 4;
      KeusTimerUtilAddTimer(&led2Timer);
      break;

    case 2:
      ledPin3.state = LED_ON;
      KeusGPIOSetPinValue(&ledPin3);
      blinkState[2] = 4;
      KeusTimerUtilAddTimer(&led3Timer);
      break;
    case 3:
      ledPin4.state = LED_ON;
      KeusGPIOSetPinValue(&ledPin4);
      blinkState[3] = 4;
      KeusTimerUtilAddTimer(&led4Timer);
      break;
    }
  }
  else
    KeusUartAckMsg(CONTROL_INVALID_STATE);
}

void saveScene(void)
{
  //Example msg : 02 05 02 00 01 01 00

  int i = 0;
  uint8 ledPointer = 3; //position of led in uart array
  uint8 freePosition = MAX_SCENE_STORAGE; //if found the value would be <MAX_SCENE_STORAGE
  bool overWrite = 0;
  uint8 overWritePosition = MAX_SCENE_STORAGE;

  //do search for overWrite position & free position
  for (i = 0; i < MAX_SCENE_STORAGE; i++)
  {
    if (arr_scene[i].id == g0_u8RecData[1])
    {
      overWritePosition = i;
      overWrite = true;
      break;
    }
    //search for free position
    if ((arr_scene[i].id == 0) && (overWritePosition > MAX_SCENE_STORAGE) && (freePosition > MAX_SCENE_STORAGE))
    {
      freePosition = i;
    }
  }

  //considering overwrite or free position
  //If overwrite position found make it free position

  if (overWritePosition < MAX_SCENE_STORAGE)
  {
    freePosition = overWritePosition;
    //clearing included array
    arr_scene[freePosition].included[0] = 0;
    arr_scene[freePosition].included[1] = 0;
    arr_scene[freePosition].included[2] = 0;
    arr_scene[freePosition].included[3] = 0;
  }

  if (freePosition < MAX_SCENE_STORAGE)
  {

    int loopLength = g0_u8RecData[2]; //no of leds present in the scene

    arr_scene[freePosition].id = g0_u8RecData[1];

    for (i = 0; i < loopLength; i++) //filling led state and included array
    {
      switch (g0_u8RecData[ledPointer])
      {
      case 0:
        arr_scene[freePosition].ledPin = g0_u8RecData[ledPointer + 1];
        arr_scene[freePosition].included[0] = 1;
        break;
      case 1:
        arr_scene[freePosition].ledPin2 = g0_u8RecData[ledPointer + 1];
        arr_scene[freePosition].included[1] = 1;
        break;
      case 2:
        arr_scene[freePosition].ledPin3 = g0_u8RecData[ledPointer + 1];
        arr_scene[freePosition].included[2] = 1;
        break;
      case 3:
        arr_scene[freePosition].ledPin4 = g0_u8RecData[ledPointer + 1];
        arr_scene[freePosition].included[3] = 1;
        break;
      default:
        KeusUartAckMsg(SAVE_INVALID_LED);
        break;
      }
      ledPointer += 2;
    }

    //if overWrite send overwrite error otherwise normal ackMsg
    if (overWrite)
      KeusUartAckMsg(SAVE_OVERWRITE);
    else
      KeusUartAckMsg(KEUS_UART_MSG_ACK);

    //nvic memory store
    osal_nv_write(SCN_ARR_NVIC_LOCATION, 0, sizeof(arr_scene), (void *)arr_scene);
  }

  else
  {
    //send memory full error
    KeusUartAckMsg(SAVE_MEMORY_FULL);
  }
}

void executeScene(void)
{ //example msg: 03 01 //execute scn id 1
  int i = 0;
  uint8 foundScene = MAX_SCENE_STORAGE;

  //Search for the scn id
  for (i = 0; i < MAX_SCENE_STORAGE; i++)
  {
    if (arr_scene[i].id == g0_u8RecData[1])
    {
      foundScene = i;
      break;
    }
  }
  if (foundScene < MAX_SCENE_STORAGE)
  {

    for (i = 0; i < 4; i++) //check included array
    {
      if (arr_scene[foundScene].included[i]) //if an array element is 1, set the corresponding state
      {
        if (i == 0)
        {
          ledPin.state = !(arr_scene[foundScene].ledPin);
          KeusGPIOSetPinValue(&ledPin);
          blinkState[0] = 0;
        }
        if (i == 1)
        {
          ledPin2.state = !(arr_scene[foundScene].ledPin2);
          KeusGPIOSetPinValue(&ledPin2);
          blinkState[1] = 0;
        }
        if (i == 2)
        {
          ledPin3.state = !(arr_scene[foundScene].ledPin3);
          KeusGPIOSetPinValue(&ledPin3);
          blinkState[2] = 0;
        }
        if (i == 3)
        {
          ledPin4.state = !(arr_scene[foundScene].ledPin4);
          KeusGPIOSetPinValue(&ledPin4);
          blinkState[3] = 0;
        }
      }
    }
    KeusUartAckMsg(KEUS_UART_MSG_ACK);
  }
  else
    KeusUartAckMsg(EXECUTE_ID_NOT_FOUND);
}

void deleteScene(void)
{
  //Example msg: 04 01
  int i = 0;
  uint8 foundScene = MAX_SCENE_STORAGE;
  //search for the id
  for (i = 0; i < MAX_SCENE_STORAGE; i++)
  {
    if (arr_scene[i].id == g0_u8RecData[1])
    {
      foundScene = i;
      break;
    }
  }
  if (foundScene < 12)
  {
    //clearing id and included array will effectively delete the scene
    arr_scene[foundScene].id = 0;
    arr_scene[foundScene].included[0] = 0;
    arr_scene[foundScene].included[1] = 0;
    arr_scene[foundScene].included[2] = 0;
    arr_scene[foundScene].included[3] = 0;

    KeusUartAckMsg(KEUS_UART_MSG_ACK);
  }
  else
    KeusUartAckMsg(DELETE_ID_NOT_FOUND);
}

void KeusUartAckMsg(uint8 data)
{
  uint8 ackMsgData[32];
  uint8 dataLen = 0;

  ackMsgData[dataLen] = KEUS_UART_MSG_INITIATOR;
  dataLen += 1;

  ackMsgData[dataLen] = 0x01; //one byte of ackdata
  dataLen += 1;

  ackMsgData[dataLen] = data;
  dataLen += 1;

  ackMsgData[dataLen] = KEUS_UART_MSG_TERMINATOR;
  dataLen += 1;

  //UART_Write(UART0, ackMsgData, dataLen);
  HalUARTWrite(HAL_UART_PORT_0, ackMsgData, dataLen);
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void zmain_ext_addr(void);
#if defined ZCL_KEY_ESTABLISH
static void zmain_cert_init(void);
#endif
static void zmain_dev_info(void);
static void zmain_vdd_check(void);

#ifdef LCD_SUPPORTED
static void zmain_lcd_init(void);
#endif

/*********************************************************************
 * @fn      main
 * @brief   First function called after startup.
 * @return  don't care
 */
int main(void)
{
  // Turn off interrupts
  osal_int_disable(INTS_ALL);

  // Initialization for board related stuff such as LEDs
  HAL_BOARD_INIT();

  // Make sure supply voltage is high enough to run
  zmain_vdd_check();

  // Initialize board I/O
  InitBoard(OB_COLD);

  // Initialze HAL drivers
  HalDriverInit();

  // Initialize NV System
  osal_nv_init(NULL);

  // Initialize the MAC
  ZMacInit();

  // Determine the extended address
  zmain_ext_addr();

#if defined ZCL_KEY_ESTABLISH
  // Initialize the Certicom certificate information.
  zmain_cert_init();
#endif

  // Initialize basic NV items
  zgInit();

#ifndef NONWK
  // Since the AF isn't a task, call it's initialization routine
  afInit();
#endif

  // Initialize the operating system
  osal_init_system();

  // Allow interrupts
  osal_int_enable(INTS_ALL);

  // Final board initialization
  InitBoard(OB_READY);

  KEUS_init();

  KEUS_loop();

  // Display information about this device
  zmain_dev_info();

#ifdef WDT_IN_PM1
  /* If WDT is used, this is a good place to enable it. */
  WatchDogEnable(WDTIMX);
#endif

  osal_start_system(); // No Return from here

  return 0; // Shouldn't get here.
} // main()

HAL_ISR_FUNCTION(halKeusPort1Isr, P1INT_VECTOR)
{
  HAL_ENTER_ISR();

  if (debounce_read)
  {
    debounce_read = false; //make sure it does not read untill the debounce timer CB sets it again

    if (P1IFG & BV(buttonPin.bit))
    {
      ledPin.state = KeusGPIOToggledState(ledPin.state);
      KeusGPIOSetPinValue(&ledPin);
    }

    if (P1IFG & BV(buttonPin2.bit))
    {
      ledPin2.state = KeusGPIOToggledState(ledPin2.state);
      KeusGPIOSetPinValue(&ledPin2);
    }

    if (P1IFG & BV(buttonPin3.bit))
    {
      ledPin3.state = KeusGPIOToggledState(ledPin3.state);
      KeusGPIOSetPinValue(&ledPin3);
    }

    if (P1IFG & BV(buttonPin4.bit))
    {
      ledPin4.state = KeusGPIOToggledState(ledPin4.state);
      KeusGPIOSetPinValue(&ledPin4);
    }

    KeusTimerUtilAddTimer(&debounceTimer);
  }
  /*
   Clear the CPU interrupt flag for Port_0
   PxIFG has to be cleared before PxIF
 */
  P1IFG = 0;
  P1IF = 0;

  CLEAR_SLEEP_MODE();
  HAL_EXIT_ISR();
}

/*********************************************************************
 * @fn      zmain_vdd_check
 * @brief   Check if the Vdd is OK to run the processor.
 * @return  Return if Vdd is ok; otherwise, flash LED, then reset
 *********************************************************************/
static void zmain_vdd_check(void)
{
  uint8 cnt = 16;

  do
  {
    while (!HalAdcCheckVdd(VDD_MIN_RUN))
      ;
  } while (--cnt);
}

/**************************************************************************************************
 * @fn          zmain_ext_addr
 *
 * @brief       Execute a prioritized search for a valid extended address and write the results
 *              into the OSAL NV system for use by the system. Temporary address not saved to NV.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
static void zmain_ext_addr(void)
{
  uint8 nullAddr[Z_EXTADDR_LEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  uint8 writeNV = TRUE;

  // First check whether a non-erased extended address exists in the OSAL NV.
  if ((SUCCESS != osal_nv_item_init(ZCD_NV_EXTADDR, Z_EXTADDR_LEN, NULL)) ||
      (SUCCESS != osal_nv_read(ZCD_NV_EXTADDR, 0, Z_EXTADDR_LEN, aExtendedAddress)) ||
      (osal_memcmp(aExtendedAddress, nullAddr, Z_EXTADDR_LEN)))
  {
    // Attempt to read the extended address from the location on the lock bits page
    // where the programming tools know to reserve it.
    HalFlashRead(HAL_FLASH_IEEE_PAGE, HAL_FLASH_IEEE_OSET, aExtendedAddress, Z_EXTADDR_LEN);

    if (osal_memcmp(aExtendedAddress, nullAddr, Z_EXTADDR_LEN))
    {
      // Attempt to read the extended address from the designated location in the Info Page.
      if (!osal_memcmp((uint8 *)(P_INFOPAGE + HAL_INFOP_IEEE_OSET), nullAddr, Z_EXTADDR_LEN))
      {
        osal_memcpy(aExtendedAddress, (uint8 *)(P_INFOPAGE + HAL_INFOP_IEEE_OSET), Z_EXTADDR_LEN);
      }
      else // No valid extended address was found.
      {
        uint8 idx;

#if !defined(NV_RESTORE)
        writeNV = FALSE; // Make this a temporary IEEE address
#endif

        /* Attempt to create a sufficiently random extended address for expediency.
         * Note: this is only valid/legal in a test environment and
         *       must never be used for a commercial product.
         */
        for (idx = 0; idx < (Z_EXTADDR_LEN - 2);)
        {
          uint16 randy = osal_rand();
          aExtendedAddress[idx++] = LO_UINT16(randy);
          aExtendedAddress[idx++] = HI_UINT16(randy);
        }
        // Next-to-MSB identifies ZigBee devicetype.
#if ZG_BUILD_COORDINATOR_TYPE && !ZG_BUILD_JOINING_TYPE
        aExtendedAddress[idx++] = 0x10;
#elif ZG_BUILD_RTRONLY_TYPE
        aExtendedAddress[idx++] = 0x20;
#else
        aExtendedAddress[idx++] = 0x30;
#endif
        // MSB has historical signficance.
        aExtendedAddress[idx] = 0xF8;
      }
    }

    if (writeNV)
    {
      (void)osal_nv_write(ZCD_NV_EXTADDR, 0, Z_EXTADDR_LEN, aExtendedAddress);
    }
  }

  // Set the MAC PIB extended address according to results from above.
  (void)ZMacSetReq(MAC_EXTENDED_ADDRESS, aExtendedAddress);
}

#if defined ZCL_KEY_ESTABLISH
/**************************************************************************************************
 * @fn          zmain_cert_init
 *
 * @brief       Initialize the Certicom certificate information.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
static void zmain_cert_init(void)
{
  uint8 certData[ZCL_KE_IMPLICIT_CERTIFICATE_LEN];
  uint8 nullData[ZCL_KE_IMPLICIT_CERTIFICATE_LEN] = {
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  (void)osal_nv_item_init(ZCD_NV_IMPLICIT_CERTIFICATE, ZCL_KE_IMPLICIT_CERTIFICATE_LEN, NULL);
  (void)osal_nv_item_init(ZCD_NV_DEVICE_PRIVATE_KEY, ZCL_KE_DEVICE_PRIVATE_KEY_LEN, NULL);

  // First check whether non-null certificate data exists in the OSAL NV. To save on code space,
  // just use the ZCD_NV_CA_PUBLIC_KEY as the bellwether for all three.
  if ((SUCCESS != osal_nv_item_init(ZCD_NV_CA_PUBLIC_KEY, ZCL_KE_CA_PUBLIC_KEY_LEN, NULL)) ||
      (SUCCESS != osal_nv_read(ZCD_NV_CA_PUBLIC_KEY, 0, ZCL_KE_CA_PUBLIC_KEY_LEN, certData)) ||
      (osal_memcmp(certData, nullData, ZCL_KE_CA_PUBLIC_KEY_LEN)))
  {
    // Attempt to read the certificate data from its corresponding location on the lock bits page.
    HalFlashRead(HAL_FLASH_IEEE_PAGE, HAL_FLASH_CA_PUBLIC_KEY_OSET, certData,
                 ZCL_KE_CA_PUBLIC_KEY_LEN);
    // If the certificate data is not NULL, use it to update the corresponding NV items.
    if (!osal_memcmp(certData, nullData, ZCL_KE_CA_PUBLIC_KEY_LEN))
    {
      (void)osal_nv_write(ZCD_NV_CA_PUBLIC_KEY, 0, ZCL_KE_CA_PUBLIC_KEY_LEN, certData);
      HalFlashRead(HAL_FLASH_IEEE_PAGE, HAL_FLASH_IMPLICIT_CERT_OSET, certData,
                   ZCL_KE_IMPLICIT_CERTIFICATE_LEN);
      (void)osal_nv_write(ZCD_NV_IMPLICIT_CERTIFICATE, 0,
                          ZCL_KE_IMPLICIT_CERTIFICATE_LEN, certData);
      HalFlashRead(HAL_FLASH_IEEE_PAGE, HAL_FLASH_DEV_PRIVATE_KEY_OSET, certData,
                   ZCL_KE_DEVICE_PRIVATE_KEY_LEN);
      (void)osal_nv_write(ZCD_NV_DEVICE_PRIVATE_KEY, 0, ZCL_KE_DEVICE_PRIVATE_KEY_LEN, certData);
    }
  }
}
#endif

/**************************************************************************************************
 * @fn          zmain_dev_info
 *
 * @brief       This displays the IEEE (MSB to LSB) on the LCD.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
static void zmain_dev_info(void)
{

#if defined(SERIAL_DEBUG_SUPPORTED) || (defined(LEGACY_LCD_DEBUG) && defined(LCD_SUPPORTED))
  uint8 i;
  uint8 *xad;
  uint8 lcd_buf[Z_EXTADDR_LEN * 2 + 1];

  // Display the extended address.
  xad = aExtendedAddress + Z_EXTADDR_LEN - 1;

  for (i = 0; i < Z_EXTADDR_LEN * 2; xad--)
  {
    uint8 ch;
    ch = (*xad >> 4) & 0x0F;
    lcd_buf[i++] = ch + ((ch < 10) ? '0' : '7');
    ch = *xad & 0x0F;
    lcd_buf[i++] = ch + ((ch < 10) ? '0' : '7');
  }
  lcd_buf[Z_EXTADDR_LEN * 2] = '\0';
  HalLcdWriteString("IEEE: ", HAL_LCD_DEBUG_LINE_1);
  HalLcdWriteString((char *)lcd_buf, HAL_LCD_DEBUG_LINE_2);
#endif
}

#ifdef LCD_SUPPORTED
/*********************************************************************
 * @fn      zmain_lcd_init
 * @brief   Initialize LCD at start up.
 * @return  none
 *********************************************************************/
static void zmain_lcd_init(void)
{
#ifdef SERIAL_DEBUG_SUPPORTED
  {
    HalLcdWriteString("TexasInstruments", HAL_LCD_DEBUG_LINE_1);

#if defined(MT_MAC_FUNC)
#if defined(ZDO_COORDINATOR)
    HalLcdWriteString("MAC-MT Coord", HAL_LCD_DEBUG_LINE_2);
#else
    HalLcdWriteString("MAC-MT Device", HAL_LCD_DEBUG_LINE_2);
#endif // ZDO
#elif defined(MT_NWK_FUNC)
#if defined(ZDO_COORDINATOR)
    HalLcdWriteString("NWK Coordinator", HAL_LCD_DEBUG_LINE_2);
#else
    HalLcdWriteString("NWK Device", HAL_LCD_DEBUG_LINE_2);
#endif // ZDO
#endif // MT_FUNC
  }
#endif // SERIAL_DEBUG_SUPPORTED
}
#endif

/*********************************************************************
*********************************************************************/
