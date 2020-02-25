#ifndef INWALLUART_H_INCLUDED
#define INWALLUART_H_INCLUDED

extern void inwalluartinit(void);
extern volatile uint32_t keusAppEvents;
extern void inwallUART_tx(uint8_t arr[], uint8_t dataLen);
extern volatile uint8_t g0_u8RecData[KEUS_UART_BUFFER];


#endif