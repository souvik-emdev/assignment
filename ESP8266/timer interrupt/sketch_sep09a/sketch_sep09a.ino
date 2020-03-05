/*
    ESP8266 Timer
    LED Blinking using Timer
*/
//#include <ESP8266WiFi.h>
//#include <Ticker.h>


//Ticker blinker;

#define LED 5  //On board LED
int state = LOW;

//=======================================================================
void ICACHE_RAM_ATTR onTimerISR(){
    //digitalWrite(LED,!(digitalRead(LED)));  //Toggle LED Pin
    //timer1_write(100);//12us
    if (state == LOW) {
    GPOS = (1 << LED);
    //gpio.write(LED, gpio.HIGH);
    state = HIGH;
    }
    else {
      GPOC = (1 << LED);
      //gpio.write(LED, gpio.LOW);
      state = LOW;
      
      }
   
}
//=======================================================================
//                               Setup
//=======================================================================
void setup()
{
    //Serial.begin(115200);
    //Serial.println("");

    pinMode(LED,OUTPUT);

    //Initialize Ticker every 0.5s
    timer1_isr_init();
    timer1_attachInterrupt(onTimerISR);
    
    timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP);
    timer1_write(1600); //120000 us
}
//=======================================================================
//                MAIN LOOP
//=======================================================================
void loop()
{
}
//=======================================================================
