#define led3 5
#define led4 4
#define led5 14
#define led6 16
#define sw3 2 //pushbutton
#define sw4 12 //pushbutton
#define sw7 13 //switch
#define sw8 15 //switch

long debouncing_time = 50; // push button stabilizing
volatile unsigned long last_microssw3;
volatile unsigned long last_microssw4;
volatile unsigned long holdtime;
volatile bool holdcheck = false;

unsigned long previousMillis[4];

volatile bool sw7state = LOW;
volatile bool sw8state = LOW;
volatile bool sw3state = LOW;
volatile bool sw4state = LOW;

volatile uint8 config1 = 0;
volatile uint8 config2 = 0;
volatile uint8 config3 = 0;
volatile uint8 config4 = 0;

uint8 cse;

void setup() {
pinMode(led3, OUTPUT);
pinMode(led4, OUTPUT);
pinMode(led5, OUTPUT);
pinMode(led6, OUTPUT);
attachInterrupt(digitalPinToInterrupt(sw7), sw7int, CHANGE);
attachInterrupt(digitalPinToInterrupt(sw8), sw8int, CHANGE);
attachInterrupt(digitalPinToInterrupt(sw3), sw3int, CHANGE);
attachInterrupt(digitalPinToInterrupt(sw4), sw4int, CHANGE);
//Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
if (sw7state == HIGH && sw8state == HIGH) {
  cse = 1;

  }
else if (sw7state == LOW && sw8state == LOW) {
  cse = 2;
}

else if (sw7state == HIGH && sw8state == LOW) {
  cse = 3;
}

else if (sw7state == LOW && sw8state == HIGH) {
  cse = 4;
}

if ((holdcheck == true) && ((micros() - holdtime)) >= 3000 * 1000) {
  holdcheck = false;
  if (digitalRead(sw3) == LOW) { 
  if (cse == 1) {
      config1 = 0;
      config2 = 1;
      config3 = 0;
      config4 = 0;
      sw3state = HIGH;
      sw4state = LOW;      
      }
   else if (cse == 2) {
      config1 = 0;
      config2 = 0;
      config3 = 0;
      config4 = 1;
      sw3state = HIGH;
      sw4state = LOW;      
      }
  }
}

if (config1 == 1) {
  if (sw3state == HIGH) {
  BlinkLed(led3, 500, 0);
  BlinkLed(led4, 1000, 1);
  BlinkLed(led5, 1500, 2);
  BlinkLed(led6, 2000, 3);
  //Serial.println("config1 sw3 high");  
  }
  else if (sw4state == HIGH) {
  BlinkLed(led3, 400, 0);
  BlinkLed(led4, 300, 1);
  BlinkLed(led5, 200, 2);
  BlinkLed(led6, 100, 3);
  //Serial.println("config1 sw4 high");  
  }
}

if (config2 == 1) {
  if (sw3state == HIGH) {
  BlinkLed(led3, 2000, 0);
  BlinkLed(led4, 1500, 1);
  BlinkLed(led5, 1000, 2);
  BlinkLed(led6, 500, 3);
  //Serial.println("config2 sw3 high");  
  }
  else if (sw4state == HIGH) {
  BlinkLed(led3, 100, 0);
  BlinkLed(led4, 200, 1);
  BlinkLed(led5, 300, 2);
  BlinkLed(led6, 400, 3);
  //Serial.println("config2 sw4 high");  
  }
}

if (config3 == 1) {
  if (sw3state == HIGH) {
  BlinkLed(led3, 100, 0);
  BlinkLed(led4, 200, 1);
  BlinkLed(led5, 300, 2);
  BlinkLed(led6, 400, 3);
  //Serial.println("config3 sw3 high");  
  }
  else if (sw4state == HIGH) {
  BlinkLed(led3, 2000, 0);
  BlinkLed(led4, 1500, 1);
  BlinkLed(led5, 1000, 2);
  BlinkLed(led6, 500, 3);
  //Serial.println("config3 sw4 high");  
  }
}

if (config4 == 1) {
  if (sw3state == HIGH) {
  BlinkLed(led3, 400, 0);
  BlinkLed(led4, 300, 1);
  BlinkLed(led5, 200, 2);
  BlinkLed(led6, 100, 3);
  //Serial.println("config4 sw3 high");  
  }
  else if (sw4state == HIGH) {
  BlinkLed(led3, 500, 0);
  BlinkLed(led4, 1000, 1);
  BlinkLed(led5, 1500, 2);
  BlinkLed(led6, 2000, 3);
  //Serial.println("config4 sw4 high");  
  }
}

if (cse == 3) {
  config1 = 0;
  config2 = 0;
  config3 = 0;
  config4 = 0;
  BlinkLed(led3, 500, 0);
  BlinkLed(led4, 600, 1);
  BlinkLed(led5, 700, 2);
  BlinkLed(led6, 800, 3);
  //Serial.println("c3 active, pushbuttons disabled");  
  }

if (cse == 4) {
  config1 = 0;
  config2 = 0;
  config3 = 0;
  config4 = 0;
  BlinkLed(led3, 20, 0);
  BlinkLed(led4, 40, 1);
  BlinkLed(led5, 60, 2);
  BlinkLed(led6, 80, 3);
  //Serial.println("c4 active, pushbuttons disabled");  
  }
//Serial.println(cse);

}



ICACHE_RAM_ATTR
void sw7int() {
    sw7state = digitalRead(sw7);   
}

void sw8int() {
    sw8state = digitalRead(sw8);    
}

void sw3int() {
 

  if ((long)(micros() - last_microssw3) >= debouncing_time * 1000) {
    
   holdtime = micros();
   holdcheck = true;   
    if ((cse == 1) && (config2 == 0)) {
      config1 = 1;
      config2 = 0;
      config3 = 0;
      config4 = 0;
      sw3state = HIGH;
      sw4state = LOW;
      }
    
    if ((cse == 2) && (config4 == 0)) {
      config1 = 0;
      config2 = 0;
      config3 = 1;
      config4 = 0;
      sw3state = HIGH;
      sw4state = LOW;
      }    
     else {
      sw3state = HIGH;
      sw4state = LOW;
      }
    }
 last_microssw3 = micros();
 }    


void sw4int() {
  if((long)(micros() - last_microssw4) >= debouncing_time * 1000) {
   sw3state = LOW;
   sw4state = HIGH;
   //Serial.println("sw4int");
   //Serial.println(config1);  
    } 
    last_microssw4 = micros();   
}

void BlinkLed (int led, int interval, int array){   
  
  //(long) can be omitted if you dont plan to blink led for very long time I think
   if (((long)millis() - previousMillis[array]) >= interval){ 
   
    previousMillis[array]= millis(); //stores the millis value in the selected array
   
    digitalWrite(led, !digitalRead(led)); //changes led state
  }
}


 
