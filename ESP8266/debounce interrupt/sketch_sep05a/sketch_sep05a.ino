const int interruptPin = 5; //GPIO 0 (Flash Button) 
const int LED = 4; //On board blue LED 
long debouncing_time = 200; //Debouncing Time in Milliseconds
volatile unsigned long last_micros;
volatile int laststate = LOW;

void setup() { 
  Serial.begin(115200); 
  pinMode(LED,OUTPUT); 
  pinMode(14,OUTPUT);
  //pinMode(interruptPin, INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, CHANGE); 
} 

void loop() 
{ 
    digitalWrite(LED,HIGH); //LED off 
    delay(1000); 
    digitalWrite(LED,LOW); //LED on 
    delay(1000); 
} 

//This program get executed when interrupt is occures i.e.change of input state
ICACHE_RAM_ATTR
void handleInterrupt() {
    if((long)(micros() - last_micros) >= debouncing_time * 1000) {
    Serial.println("Interrupt Detected"); 
    laststate = ! laststate;                   
    digitalWrite(14, laststate);
    last_micros = micros(); 
    }
    
}
