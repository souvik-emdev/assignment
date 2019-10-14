#define data_direction 15
#define data_pin 12
#define clk 13
//Transmitter int variables 
char test[] = "Hello ESP";
volatile int sentcount;
volatile int bitpoint;
boolean toggle2 = 1;

//Receiver int variables
char receive_buffer[100] = "0";
char received_data[100] = "0";
volatile int buffer_pointer = 0;
volatile int bytes[8];
volatile int bytes_pointer = 0;

void transmit(void){
 detachInterrupt(digitalPinToInterrupt(clk));
 pinMode(data_pin, OUTPUT);
 pinMode(data_direction, OUTPUT);
 pinMode(clk, OUTPUT);
 digitalWrite(data_direction, HIGH);
 sentcount = 0;
 bitpoint = 7;
 
 //set timer1 interrupt
 timer1_isr_init();
 timer1_attachInterrupt(transmit_ISR);
 timer1_enable(TIM_DIV256, TIM_EDGE, TIM_LOOP);
 timer1_write(1250); //250us = 4khz frequency
  }

void transmit_ISR(void){
  if (toggle2){
    digitalWrite(data_pin, bitRead(test[sentcount], bitpoint));
    Serial.print(bitRead(test[sentcount], bitpoint));
    digitalWrite(clk,HIGH);
    if (bitpoint == 0){
    sentcount++;
    bitpoint = 8;
    }
    bitpoint--; 
    if (sentcount == strlen(test)){
//    TCCR2A = 0;// set entire TCCR2A register to 0
//    TCCR2B = 0;// same for TCCR2B
//    TCNT2  = 0;//initialize counter value to 0
//    TIMSK2 = 0;//disable timer int
    timer1_disable();
    timer1_detachInterrupt();
    digitalWrite(data_direction, LOW);
    pinMode(data_pin, INPUT);
    pinMode(data_direction, INPUT);
    attachInterrupt(digitalPinToInterrupt(clk), receive_ISR, RISING);
    }
    toggle2 = 0;   
  }
  else{
    digitalWrite(clk,LOW);
    toggle2 = 1;
  }   
}  
  

void receive_ISR(void){  
  bytes[bytes_pointer] = digitalRead(data_pin);
  Serial.print(digitalRead(data_pin));
  if (bytes_pointer == 7){
        int temp;
        bytes_pointer = -1;
        temp = 0;
        for (int j = 0; j < 8; j++) {
           uint8 bitshiftedVal = (bytes[j] << (7-j));
           temp |= bitshiftedVal;           
             } 
        receive_buffer[buffer_pointer] = char(temp);
        buffer_pointer++;
      }
  bytes_pointer++;
}

void setup() {
  // put your setup code here, to run once:
attachInterrupt(digitalPinToInterrupt(clk), receive_ISR, RISING);
pinMode(data_pin, INPUT);
pinMode(data_direction, INPUT);
Serial.begin(115200);
transmit();
}

void loop() {
  // put your main code here, to run repeatedly:
if ((receive_buffer[0] != 0) && (digitalRead(data_direction) == LOW)){
  memset(received_data, 0, sizeof(received_data));
  strcpy(received_data, receive_buffer);
  memset(receive_buffer, 0, sizeof(receive_buffer));
  buffer_pointer = 0;
  bytes_pointer = 0;
  Serial.println("");
  Serial.print("Received_data:");
  Serial.println(received_data);
  }
}
