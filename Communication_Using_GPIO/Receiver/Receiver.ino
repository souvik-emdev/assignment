#define send_enable 15
#define data 12
#define clk 13
#define led1 5
#define led2 4
#define led3 14
#define led4 16
bool previous_clk = LOW;
String receive_buffer;
uint8 pointer = 0;
uint8 bytes[8];
bool flag;
char led_no;
char led_state;
bool state;

void setup() {

pinMode(send_enable, INPUT);
pinMode(data, INPUT);
pinMode(clk, INPUT);
pinMode(led1, OUTPUT);
pinMode(led2, OUTPUT);
pinMode(led3, OUTPUT);
pinMode(led4, OUTPUT);
Serial.begin(115200);
}

void loop() {
while (digitalRead(send_enable) == HIGH) {
  flag = true;
  if (digitalRead(clk) == LOW){
    previous_clk = LOW;
    }
  if ((digitalRead(clk) == HIGH) && (previous_clk == LOW)){
        bytes[pointer] = digitalRead(data);
          uint8 temp;
          if (pointer == 7) {
          pointer = -1;
          temp = 0;
          for (int j = 0; j < 8; j++) {
            uint8 bitshiftedVal = (bytes[j] << (7-j));
            temp |= bitshiftedVal;           
              }
              
          receive_buffer.concat(char(temp));
           //or Serial.print(char(temp)) 
          } 
       pointer = pointer+1;
       previous_clk = HIGH; 
       //delayMicroseconds(3);   
  }
    }
 pointer = 0;
 if (flag) { 
 Serial.println(receive_buffer);

 for (int k=0; k<(receive_buffer.length()); k++){
  if ((k == 0) || (k == 4) || (k == 8) || (k == 12)){
    led_no = receive_buffer.charAt(k);
    }
  if ((k == 2) || (k == 6) || (k == 10) || (k == 14)){
    led_state = receive_buffer.charAt(k);
    if (led_state == '0'){
      state = LOW;
      }
     else if (led_state == '1'){
      state = HIGH;
     
      } 
    }
  if ((led_no == '1') && ((led_state == '0') || (led_state == '1'))){
    digitalWrite(led1, state);
    }  
  if ((led_no == '2') && ((led_state == '0') || (led_state == '1'))){
    digitalWrite(led2, state);
    } 
  if ((led_no == '3') && ((led_state == '0') || (led_state == '1'))){
    digitalWrite(led3, state);
    }
  if ((led_no == '4') && ((led_state == '0') || (led_state == '1'))){
    digitalWrite(led4, state);
    } 
  }

  
 flag = false; 
 receive_buffer = "";
 }
   
  }
