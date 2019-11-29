#include "keus_gpio_util.h"
#include "keus_timer_util.h"
#include "hal_mcu.h"

extern void ledTimerCbk(uint8 timerId);
uint8 toggledState = GPIO_LOW ;
bool toggle = true;

KeusGPIOPin ledPin = {1, 4, GPIO_OUTPUT, false, GPIO_LOW};
KeusGPIOPin buttonPin = {1, 2, GPIO_INPUT, true, GPIO_LOW};

KeusTimerConfig intervalTimer = {
  &ledTimerCbk,
  2000,
  true,
  -1,
  0
};

void ledTimerCbk(uint8 timerId) {
  ledPin.state = KeusGPIOToggledState(ledPin.state);
  KeusGPIOSetPinValue(&ledPin);
}

int main( void )
{
  HAL_ENABLE_INTERRUPTS(); // enable global interrupts
  KeusTimerUtilInit();
  KeusTimerUtilStartTimer();
  KeusGPIOSetDirection(&ledPin);
  KeusGPIOSetDirection(&buttonPin);
  KeusGPIOEdgeConfiguration (&buttonPin,GPIO_RISING_EDGE);
  KeusGPIOInterruptEnable(&buttonPin);
  //KeusTimerUtilAddTimer(&intervalTimer);

  KeusGPIOReadPinValue(&buttonPin);

  KeusGPIOSetPinValue(&ledPin);
  
  while (true) {
   

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
//    if(buttonPin.state == GPIO_LOW){
//      if(toggle) {
//        ledPin.state = KeusGPIOToggledState(ledPin.state);
//        toggle = false;
//        KeusGPIOSetPinValue(&ledPin);
//      }
//    }
//    else toggle = true ;
      
    
    
  }
  

}

//*******************************************************************************


HAL_ISR_FUNCTION( halKeusPort1Isr, P1INT_VECTOR )
{
  HAL_ENTER_ISR();

  if (P1IFG & BV(buttonPin.bit)) {
    //check debounce (if debounce)
  ledPin.state = KeusGPIOToggledState(ledPin.state);
  KeusGPIOSetPinValue(&ledPin);
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