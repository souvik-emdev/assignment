#ifndef KEUSACTIONS_H_INCLUDED
#define KEUSACTIONS_H_INCLUDED

/* ^^ these are the include guards */

/* Prototypes for the functions */

#define LED1 P22
#define LED2 P23
#define LED3 P24
#define LED4 P25

#define CONFIGNORMAL 1
#define CONFIGDIM 2
#define CONFIGFAN 3

#define LEDID1 1
#define LEDID2 2
#define LEDID3 3
#define LEDID4 4

#define LED_HIGH 0
#define LED_LOW 1

#define TASK_CONFIG_SWITCH 1
#define TASK_GET_SWITCH_STATE 2
#define TASK_GET_CONFIG 3
#define TASK_EXECUTE_SWITCH_STATE 4
#define TASK_UART_ACK 5

#define GETSWITCHSTATEREPLY 0x06
#define GETCONFIGREPLY 0x07
#define REPORTSWITCHSTATE 0x01

#define KEUS_PWM_CHL0 0
#define KEUS_PWM_CHL1 1
#define KEUS_PWM_CHL2 2
#define KEUS_PWM_CHL3 3
#define KEUS_PWM_FREQUENCY 900

#define MAX_UART_DATA_BUFFER 10
#define UARTRETRYBUFFER 6

#define MAX_NUMBER_LED 4

uint8_t commandId, txnId, uartDataBuffer[MAX_UART_DATA_BUFFER], retryBuffer[UARTRETRYBUFFER], retryDataLen;

extern volatile uint8_t g0_u8RecData[];
extern uint32_t keusAppEvents;
extern void UART_tx(uint8_t arr[], uint8_t datalen);

void parseUart(void);
void taskHandler(void);
void configSwitch(void);
void getSwitchState(void);
void getConfig(void);
void TMR0_IRQHandler(void);
void retryGettingAck(uint8_t arr[], uint8_t dataLen);
void uartAck(void);
long map(long x, long in_min, long in_max, long out_min, long out_max);
void executeSwitchState(void);
void setOutputPins(uint8_t pin, uint8_t mode);
void setPwmDuty(uint8_t ledno, uint8_t duty);
void updateTxnId(void);
void keus_config_switch_init(void);

struct led
{
    uint8_t state;
    uint8_t config;
};

struct led arr_led[MAX_NUMBER_LED];

void toggleLed(uint8_t);

#endif