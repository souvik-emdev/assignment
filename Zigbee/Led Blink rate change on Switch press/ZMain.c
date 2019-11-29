#include "keus_gpio_util.h"
#include "keus_timer_util.h"
#include "hal_mcu.h"

extern void ledTimerCbk(uint8 timerId);
extern void debounceTimerCbk(uint8 timerId);
// uint8 toggledState = GPIO_LOW;
// bool toggle = true;
volatile bool debounce_read = true;
volatile uint8 timerValue = 0;

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
    2000,
    true,
    -1,
    0};

KeusTimerConfig intervalTimer2 = {
    &ledTimerCbk,
    1000,
    true,
    -1,
    0};

KeusTimerConfig intervalTimer3 = {
    &ledTimerCbk,
    500,
    true,
    -1,
    0};

void ledTimerCbk(uint8 timerId)
{
  ledPin.state = KeusGPIOToggledState(ledPin.state);
  KeusGPIOSetPinValue(&ledPin);
  ledPin2.state = KeusGPIOToggledState(ledPin2.state);
  KeusGPIOSetPinValue(&ledPin2);
  ledPin3.state = KeusGPIOToggledState(ledPin3.state);
  KeusGPIOSetPinValue(&ledPin3);
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
  if (timerValue == 1)
    KeusTimerUtilAddTimer(&intervalTimer);
  else if (timerValue == 2)
    KeusTimerUtilAddTimer(&intervalTimer2);
  else
    KeusTimerUtilAddTimer(&intervalTimer3);
}

int main(void)
{
  HAL_ENABLE_INTERRUPTS(); // enable global interrupts
  KeusTimerUtilInit();
  KeusTimerUtilStartTimer();
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

    if (timerValue == 1)
      KeusTimerUtilRemoveTimer(&intervalTimer);
    else if (timerValue == 2)
      KeusTimerUtilRemoveTimer(&intervalTimer2);
    else
      KeusTimerUtilRemoveTimer(&intervalTimer3);

    if (P1IFG & BV(buttonPin.bit))
    {
      // ledPin.state = KeusGPIOToggledState(ledPin.state);
      // KeusGPIOSetPinValue(&ledPin);
      timerValue = 1;
    }

    if (P1IFG & BV(buttonPin2.bit))
    {
      // ledPin2.state = KeusGPIOToggledState(ledPin2.state);
      // KeusGPIOSetPinValue(&ledPin2);
      timerValue = 2;
    }

    if (P1IFG & BV(buttonPin3.bit))
    {
      // ledPin3.state = KeusGPIOToggledState(ledPin3.state);
      // KeusGPIOSetPinValue(&ledPin3);
      timerValue = 3;
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