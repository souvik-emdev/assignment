#include "keus_gpio_util.h"
#include "keus_timer_util.h"
#include "hal_mcu.h"

extern void ledTimerCbk(uint8 timerId);
extern void debounceTimerCbk(uint8 timerId);
uint8 toggledState = GPIO_LOW;
bool toggle = true;
volatile bool debounce_read = true;

//KeusGPIOPin ledPin = {1, 4, GPIO_OUTPUT, false, GPIO_LOW};
//KeusGPIOPin buttonPin = {0, 7, GPIO_INPUT, true, GPIO_LOW};
KeusGPIOPin ledPin = {0, 5, GPIO_OUTPUT, false, GPIO_LOW};
KeusGPIOPin buttonPin = {1, 5, GPIO_INPUT, true, GPIO_LOW};
KeusGPIOPin buttonPin2 = {1, 4, GPIO_INPUT, true, GPIO_LOW};

KeusTimerConfig intervalTimer = {
    &ledTimerCbk,
    2000,
    true,
    -1,
    0};

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

void ledTimerCbk(uint8 timerId)
{
  ledPin.state = KeusGPIOToggledState(ledPin.state);
  KeusGPIOSetPinValue(&ledPin);
  
}

int main(void)
{
  HAL_ENABLE_INTERRUPTS(); // enable global interrupts
  KeusTimerUtilInit();
  KeusTimerUtilStartTimer();
  KeusGPIOSetDirection(&ledPin);
  KeusGPIOSetDirection(&buttonPin);
  KeusGPIOSetDirection(&buttonPin2);

  KeusGPIOInterruptEnable(&buttonPin2);

  // KeusTimerUtilAddTimer(&intervalTimer);
  //KeusTimerUtilAddTimer(&debounceTimer);

  KeusGPIOReadPinValue(&buttonPin);

  KeusGPIOSetPinValue(&ledPin);

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
    KeusGPIOReadPinValue(&buttonPin);
    if (buttonPin.state == GPIO_LOW)
    {

      if (debounce_read)
      {
        debounce_read = false;
        ledPin.state = KeusGPIOToggledState(ledPin.state);
        KeusGPIOSetPinValue(&ledPin);
        KeusTimerUtilAddTimer(&debounceTimer);
      }
      
    }
  }
}