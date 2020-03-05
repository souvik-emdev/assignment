#define blueLed 5   //
#define greenLed 4  //pin for each led

unsigned long previousMillis[2]; //[x] = number of leds

void setup() { 
  pinMode(blueLed, OUTPUT);   
  pinMode(greenLed, OUTPUT);  
}
void loop() {
 BlinkLed(blueLed, 100, 0);   //BlinkLed( which led, interval, one of the stored prevMillis
 BlinkLed(greenLed, 200, 1);  //last parameters must be different for each led
}

void BlinkLed (int led, int interval, int array){   
  
  //(long) can be omitted if you dont plan to blink led for very long time I think
   if (((long)millis() - previousMillis[array]) >= interval){ 
   
    previousMillis[array]= millis(); //stores the millis value in the selected array
   
    digitalWrite(led, !digitalRead(led)); //changes led state
  }}
