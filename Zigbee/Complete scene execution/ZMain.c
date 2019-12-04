/**************************************************************************************************
  Filename:       ZMain.c
  Revised:        $Date: 2010-09-17 16:25:30 -0700 (Fri, 17 Sep 2010) $
  Revision:       $Revision: 23835 $

  Description:    Startup and shutdown code for ZStack
  Notes:          This version targets the Chipcon CC2530


  Copyright 2005-2010 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED �AS IS� WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
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

#define INVALID_DATA 0XFF
#define CONTROL_INVALID_STATE 0X1F
#define CONTROL_INVALID_LED 0X2F
#define SAVE_OVERWRITE 0X3F
#define SAVE_MEMORY_FULL 0X4F
#define SAVE_INVALID_LED 0X5F
#define EXECUTE_ID_NOT_FOUND 0X6F
#define DELETE_ID_NOT_FOUND 0X7F

uint8 message_received = 0;
uint8 g0_u8RecData[KEUS_UART_BUFFER] = {0};
uint8 g0_bufferCounter = 0;
uint8 g0_shouldSaveToBuffer = 0;
uint8 g0_endMessageIndex = 0;

KeusGPIOPin ledPin = {0, 0, GPIO_OUTPUT, false, GPIO_HIGH};
KeusGPIOPin ledPin2 = {0, 1, GPIO_OUTPUT, false, GPIO_HIGH};
KeusGPIOPin ledPin3 = {0, 4, GPIO_OUTPUT, false, GPIO_LOW};
KeusGPIOPin ledPin4 = {0, 5, GPIO_OUTPUT, false, GPIO_HIGH};
KeusGPIOPin buttonPin = {1, 2, GPIO_INPUT, true, GPIO_LOW};
KeusGPIOPin buttonPin2 = {1, 3, GPIO_INPUT, true, GPIO_LOW};
KeusGPIOPin buttonPin3 = {1, 4, GPIO_INPUT, true, GPIO_LOW};
KeusGPIOPin buttonPin4 = {1, 5, GPIO_INPUT, true, GPIO_LOW};

void initUart(void);
void uartRxCb(uint8 port, uint8 event);
void KEUS_delayms(uint16 ms);
void KEUS_init(void);
void KEUS_start(void);
void KEUS_loop(void);
void debounceTimerCbk(uint8 timerId);
void ledControl(void);
void saveScene(void);
void executeScene(void);
void deleteScene(void);
void KeusUartAckMsg(uint8);

volatile bool debounce_read = true;

uint8 storedSceneCount = 0;

struct scene
{
  uint8 id;
  uint8 ledPin;
  uint8 ledPin2;
  uint8 ledPin3;
  uint8 ledPin4;
  uint8 included[4];
};

struct scene arr_scene[10];

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
  uint8 u8InChar;

  while (Hal_UART_RxBufLen(port))
  {
    // Read one byte from UART to ch
    HalUARTRead(port, &u8InChar, 1);

    // if (u8InChar == 's')
    // {
    //   ledPin.state = GPIO_LOW;
    //   KeusGPIOSetPinValue(&ledPin);
    // }
    // else if (u8InChar == 'r')
    // {
    //   ledPin.state = GPIO_HIGH;
    //   KeusGPIOSetPinValue(&ledPin);
    // }

    if (u8InChar == KEUS_UART_MSG_INITIATOR && g0_endMessageIndex == 0)
    {
      g0_shouldSaveToBuffer = 1;
      g0_bufferCounter = 0;
    }
    else if (g0_endMessageIndex == 0 && g0_shouldSaveToBuffer)
    {
      g0_endMessageIndex = u8InChar; //u8InChar - 1;
    }
    else if (u8InChar == KEUS_UART_MSG_TERMINATOR && g0_endMessageIndex == g0_bufferCounter)
    {
      g0_shouldSaveToBuffer = 0;
      g0_endMessageIndex = 0;

      message_received = 1;
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

  HalUARTWrite(HAL_UART_PORT_0, "KEUS INIT", (byte)osal_strlen("KEUS INIT"));
}

void KEUS_loop()
{
  while (1)
  {
    HalUARTPoll();
    //KEUS_delayms(1000);
    if (message_received)
    {

      switch (g0_u8RecData[0])
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
      default:
        KeusUartAckMsg(INVALID_DATA);
        break;
      }
      message_received = 0;
    }
  }
}

void ledControl(void)
{
  //Example msg : 28 03 01 03 01 29
  uint8 ledState = 0;

  if ((g0_u8RecData[2] == 1) || (g0_u8RecData[2] == 0))
  {
    uint8 ledNo = g0_u8RecData[1];
    if (g0_u8RecData[2])
      ledState = GPIO_LOW;
    else
      ledState = GPIO_HIGH;
    switch (ledNo)
    {
    case 0:
      ledPin.state = ledState;
      KeusGPIOSetPinValue(&ledPin);
      KeusUartAckMsg(KEUS_UART_MSG_ACK);
      break;

    case 1:
      ledPin2.state = ledState;
      KeusGPIOSetPinValue(&ledPin2);
      KeusUartAckMsg(KEUS_UART_MSG_ACK);
      break;

    case 2:
      ledPin3.state = ledState;
      KeusGPIOSetPinValue(&ledPin3);
      KeusUartAckMsg(KEUS_UART_MSG_ACK);
      break;

    case 3:
      ledPin4.state = ledState;
      KeusGPIOSetPinValue(&ledPin4);
      KeusUartAckMsg(KEUS_UART_MSG_ACK);
      break;
    default:
      KeusUartAckMsg(CONTROL_INVALID_LED);
      break;
    }
  }
  else
    KeusUartAckMsg(CONTROL_INVALID_STATE);
}

void saveScene(void)
{
  //Example msg : 28 07
  // 02 05 02 00 01 01 00 29

  int i = 0;
  uint8 ledPointer = 3;
  uint8 freePosition = 11;
  bool overWrite = 0;
  uint8 overWritePosition = 11;

  //do search for overWrite position & free position
  for (i = 0; i < 10; i++)
  {
    if (arr_scene[i].id == g0_u8RecData[1])
    {
      overWritePosition = i;
      overWrite = 1;
      break;
    }
    if ((arr_scene[i].id == 0) && (overWritePosition > 10) && (freePosition > 10))
    {
      freePosition = i;
    }
  }

  //considering overwrite/free

  if (overWritePosition < 10)
  {
    freePosition = overWritePosition;
    arr_scene[freePosition].included[0] = 0;
    arr_scene[freePosition].included[1] = 0;
    arr_scene[freePosition].included[2] = 0;
    arr_scene[freePosition].included[3] = 0;
  }

  if (freePosition < 10)
  {

    int loopLength = g0_u8RecData[2];

    arr_scene[freePosition].id = g0_u8RecData[1];
    for (i = 0; i < loopLength; i++)
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
  }

  else
  {
    //send memory full error
    KeusUartAckMsg(SAVE_MEMORY_FULL);
  }
  storedSceneCount++; // not using currently
}

void executeScene(void)
{ //example msg 28 02\ 03 01/ 29
  int i = 0;
  uint8 foundScene = 12;
  for (i = 0; i < 10; i++)
  {
    if (arr_scene[i].id == g0_u8RecData[1])
    {
      foundScene = i;
      break;
    }
  }
  if (foundScene < 12)
  {

    for (i = 0; i < 4; i++)
    {
      if (arr_scene[foundScene].included[i])
      {
        if (i == 0)
        {
          ledPin.state = !(arr_scene[foundScene].ledPin);
          KeusGPIOSetPinValue(&ledPin);
        }
        if (i == 1)
        {
          ledPin2.state = !(arr_scene[foundScene].ledPin2);
          KeusGPIOSetPinValue(&ledPin2);
        }
        if (i == 2)
        {
          ledPin3.state = !(arr_scene[foundScene].ledPin3);
          KeusGPIOSetPinValue(&ledPin3);
        }
        if (i == 3)
        {
          ledPin4.state = !(arr_scene[foundScene].ledPin4);
          KeusGPIOSetPinValue(&ledPin4);
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
  //28 02 04 01 29
  int i = 0;
  uint8 foundScene = 12;
  for (i = 0; i < 10; i++)
  {
    if (arr_scene[i].id == g0_u8RecData[1])
    {
      foundScene = i;
      break;
    }
  }
  if (foundScene < 12)
  {
    //arr_scene[foundScene] = {{0}};
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

  ackMsgData[dataLen] = 0x01;
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
    debounce_read = false;

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
