#include "Nano100Series.h"
#include "inwallconstants.h"
#include <stdio.h>

volatile uint32_t keusAppEvents;

volatile uint8_t g0_u8RecData[KEUS_UART_BUFFER] = {0}, g0_bufferCounter = 0, g0_shouldSaveToBuffer = 0, g0_endMessageIndex = 0;


void inwalluartinit(void)
{
    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART1, 115200);

    UART_ENABLE_INT(UART1, UART_IER_RDA_IE_Msk);
    NVIC_EnableIRQ(UART1_IRQn);
}

void UART1_IRQHandler(void)
{
    uint8_t u8InChar = 0xFF;
    uint32_t u32IntSts = UART1->ISR;

    if (u32IntSts & UART_ISR_RDA_IS_Msk)
    {
        /* Get all the input characters */
        while (UART_IS_RX_READY(UART1))
        {

            /* Get the character from UART Buffer */
            u8InChar = UART_READ(UART1); /* Rx trigger level is 1 byte*/

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

                //message_received = g0_bufferCounter; //SET THE FLAG HERE
                keusAppEvents |= INWALL_UART;
            }
            else if (g0_bufferCounter > g0_endMessageIndex)
            {
                g0_shouldSaveToBuffer = 0;
                g0_endMessageIndex = 0;
            }
            else if (g0_shouldSaveToBuffer)
                g0_u8RecData[g0_bufferCounter++] = u8InChar;
            else
                g0_endMessageIndex = 0;
        }
    }
}

void inwallUART_tx(uint8_t arr[], uint8_t dataLen)
{
  uint8_t sendBuffer[25], bufferLength = 0;
  sendBuffer[0] = KEUS_UART_MSG_INITIATOR;
  bufferLength++;
  sendBuffer[1] = dataLen;
  bufferLength++;
  for (uint8_t i = 0; i < dataLen; i++)
  {
    sendBuffer[i + 2] = arr[i];
    bufferLength++;
  }
  sendBuffer[bufferLength++] = KEUS_UART_MSG_TERMINATOR;

  UART_Write(UART1, sendBuffer, bufferLength);
}