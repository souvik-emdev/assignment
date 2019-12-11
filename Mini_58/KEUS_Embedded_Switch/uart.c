include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "Mini58Series.h"
#include "UART.h"

void UART0_IRQHandler(void)
{
  uint8_t u8InChar = 0x73;
  uint32_t u32IntSts = UART0->INTSTS;

  if (u32IntSts & UART_INTSTS_RDAINT_Msk)
  {
    /* Get all the input characters */
    while (UART_IS_RX_READY(UART0))
    {

      /* Get the character from UART Buffer */
      u8InChar = UART_READ(UART0); /* Rx trigger level is 1 byte*/

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

        message_received = g0_bufferCounter; //SET THE FLAG HERE
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