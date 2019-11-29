#include "keus_gpio_util.h"
#include "keus_timer_util.h"
#include "hal_mcu.h"
#include "hal_uart.h"
#include "osal.h"

#define LED_1_TIME 20 //20*100 = 2000ms
#define LED_2_TIME 10 //10*100 = 1000ms
#define LED_3_TIME 5  //5*100 = 500ms

// void initUart(void);
// void uartRxCb( uint8 port, uint8 event );

extern void ledTimerCbk(uint8 timerId);
extern void debounceTimerCbk(uint8 timerId);
// uint8 toggledState = GPIO_LOW;
// bool toggle = true;
volatile bool debounce_read = true;
volatile uint8 timerValue = 0;
volatile uint8 Led1Counter = 0;
volatile uint8 Led2Counter = 0;
volatile uint8 Led3Counter = 0;
volatile bool led1_Enable = false;
volatile bool led2_Enable = false;
volatile bool led3_Enable = false;

//KeusGPIOPin ledPin = {1, 4, GPIO_OUTPUT, false, GPIO_LOW};
//KeusGPIOPin buttonPin = {0, 7, GPIO_INPUT, true, GPIO_LOW};
KeusGPIOPin ledPin = {0, 0, GPIO_OUTPUT, false, GPIO_LOW};
KeusGPIOPin ledPin2 = {0, 1, GPIO_OUTPUT, false, GPIO_LOW};
KeusGPIOPin ledPin3 = {0, 4, GPIO_OUTPUT, false, GPIO_LOW};
KeusGPIOPin buttonPin = {1, 2, GPIO_INPUT, true, GPIO_LOW};
KeusGPIOPin buttonPin2 = {1, 3, GPIO_INPUT, true, GPIO_LOW};
KeusGPIOPin buttonPin3 = {1, 4, GPIO_INPUT, true, GPIO_LOW};

KeusTimerConfig intervalTimer = {
    &ledTimerCbk,
    100,
    true,
    -1,
    0};

void ledTimerCbk(uint8 timerId)
{
  if (led1_Enable)
  Led1Counter++;
  if (led2_Enable)
  Led2Counter++;
  if (led3_Enable)
  Led3Counter++;
  if (Led1Counter >= LED_1_TIME)
  {
    ledPin.state = KeusGPIOToggledState(ledPin.state);
    KeusGPIOSetPinValue(&ledPin);
    Led1Counter = 0;
  }
  if (Led2Counter >= LED_2_TIME)
  {
    ledPin2.state = KeusGPIOToggledState(ledPin2.state);
    KeusGPIOSetPinValue(&ledPin2);
    Led2Counter = 0;
  }
  if (Led3Counter >= LED_3_TIME)
  {
    ledPin3.state = KeusGPIOToggledState(ledPin3.state);
    KeusGPIOSetPinValue(&ledPin3);
    Led3Counter = 0;
  }
}

KeusTimerConfig debounceTimer = {
    &debounceTimerCbk,
    200,
    true,
    -1,
    0};

void debounceTimerCbk(uint8 timerId)
{
  debounce_read = true;
  // ledPin.state = KeusGPIOToggledState(ledPin.state);
  // KeusGPIOSetPinValue(&ledPin);
  KeusTimerUtilRemoveTimer(&debounceTimer);
  
}

int main(void)
{
  HAL_ENABLE_INTERRUPTS(); // enable global interrupts
  KeusTimerUtilInit();
  KeusTimerUtilStartTimer();
  KeusTimerUtilAddTimer(&intervalTimer);

  KeusGPIOSetDirection(&ledPin);
  KeusGPIOSetDirection(&ledPin2);
  KeusGPIOSetDirection(&ledPin3);
  KeusGPIOSetDirection(&buttonPin);
  KeusGPIOSetDirection(&buttonPin2);
  KeusGPIOSetDirection(&buttonPin3);

  KeusGPIOEdgeConfiguration(&buttonPin, GPIO_RISING_EDGE);
  KeusGPIOEdgeConfiguration(&buttonPin2, GPIO_RISING_EDGE);
  KeusGPIOEdgeConfiguration(&buttonPin3, GPIO_RISING_EDGE);
  KeusGPIOInterruptEnable(&buttonPin);
  KeusGPIOInterruptEnable(&buttonPin2);
  KeusGPIOInterruptEnable(&buttonPin3);

  // initUart();
  // HalUARTWrite( HAL_UART_PORT_0, "KEUS INIT", (byte)osal_strlen("KEUS INIT"));

  // KeusTimerUtilAddTimer(&intervalTimer);
  //KeusTimerUtilAddTimer(&debounceTimer);

  while (true)
  {

    //state of pins
    //    ledPin.state = toggledState;

    // high when button held
    // KeusGPIOReadPinValue(&buttonPin);
    //  if (ledPin.state != buttonPin.state)
    //    ledPin.state = buttonPin.state;
    //  KeusGPIOSetPinValue(&ledPin);

    // high when button held
    //    if (buttonPin.state == GPIO_LOW) {
    //      ledPin.state = GPIO_HIGH;
    //      KeusGPIOSetPinValue(&ledPin);
    //    } else if (buttonPin.state == GPIO_HIGH) {
    //       ledPin.state = GPIO_LOW;
    //      KeusGPIOSetPinValue(&ledPin);
    //    }

    // toggle when button pressed `
    // if (buttonPin.state == GPIO_LOW)
    // {
    //   if (toggle)
    //   {
    //     ledPin.state = KeusGPIOToggledState(ledPin.state);
    //     toggle = false;
    //     KeusGPIOSetPinValue(&ledPin);
    //   }
    // }
    // else
    //   toggle = true;

    //debounce toggle
    // KeusGPIOReadPinValue(&buttonPin);
    // if (buttonPin.state == GPIO_LOW)
    // {

    //   if (debounce_read)
    //   {
    //     debounce_read = false;
    //     ledPin.state = KeusGPIOToggledState(ledPin.state);
    //     KeusGPIOSetPinValue(&ledPin);
    //     KeusTimerUtilAddTimer(&debounceTimer);
    //   }

    // }
  }
}

HAL_ISR_FUNCTION(halKeusPort1Isr, P1INT_VECTOR)
{
  HAL_ENTER_ISR();

  if (debounce_read)
  {
    debounce_read = false;

    if (P1IFG & BV(buttonPin.bit))
    {
      // ledPin.state = KeusGPIOToggledState(ledPin.state);
      // KeusGPIOSetPinValue(&ledPin);
      led1_Enable = !(led1_Enable);
      Led1Counter = 0;
      if (!(led1_Enable)) {
        ledPin.state = GPIO_HIGH;        
      }
      else ledPin.state = GPIO_LOW;
      KeusGPIOSetPinValue(&ledPin);
    }

    if (P1IFG & BV(buttonPin2.bit))
    {
      // ledPin2.state = KeusGPIOToggledState(ledPin2.state);
      // KeusGPIOSetPinValue(&ledPin2);
      led2_Enable = !(led2_Enable);
      Led2Counter = 0;
      if (!(led2_Enable)) {
        ledPin2.state = GPIO_HIGH;        
      }
      else ledPin2.state = GPIO_LOW;
      KeusGPIOSetPinValue(&ledPin2);
    }

    if (P1IFG & BV(buttonPin3.bit))
    {
      // ledPin3.state = KeusGPIOToggledState(ledPin3.state);
      // KeusGPIOSetPinValue(&ledPin3);
      led3_Enable = !(led3_Enable);
      Led3Counter = 0;
      if (!(led3_Enable)) {
        ledPin3.state = GPIO_HIGH;
      }
      else ledPin3.state = GPIO_LOW;
      KeusGPIOSetPinValue(&ledPin3);
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

// void initUart() {
//  halUARTCfg_t uartConfig;
//  uartConfig.configured           = TRUE;
//  uartConfig.baudRate             = HAL_UART_BR_115200;
//  uartConfig.flowControl          = FALSE;
//  uartConfig.flowControlThreshold = 48;
//  uartConfig.rx.maxBufSize        = 128;
//  uartConfig.tx.maxBufSize        = 128;
//  uartConfig.idleTimeout          = 6;  
//  uartConfig.intEnable            = TRUE;
//  uartConfig.callBackFunc         = uartRxCb;
//  HalUARTOpen (HAL_UART_PORT_0, &uartConfig);
// }

// void uartRxCb( uint8 port, uint8 event ) {
//  uint8  ch;

//    while (Hal_UART_RxBufLen(port))
//  {
//    // Read one byte from UART to ch
//    HalUARTRead (port, &ch, 1);
   
//    if (ch == 's') {
//      ledPin.state = GPIO_LOW;
//      KeusGPIOSetPinValue(&ledPin); 
//    } else if (ch == 'r') {
//      ledPin.state = GPIO_HIGH;
//      KeusGPIOSetPinValue(&ledPin);
//    }
//  }
// }
