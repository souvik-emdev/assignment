#ifndef BUTTON_H_INCLUDED
#define BUTTON_H_INCLUDED

/* ^^ these are the include guards */

/* Prototypes for the functions */
#define KEUS_BUTTON1 0x1
#define KEUS_BUTTON2 0x2
#define KEUS_BUTTON3 0x4
#define KEUS_BUTTON4 0x8
#define KEUS_UART 0x10

extern uint32_t keusAppEvents;

void GPIO234_IRQHandler(void);
void EINT1_IRQHandler(void);
void EINT0_IRQHandler(void);
void keus_button_init(void);

#endif