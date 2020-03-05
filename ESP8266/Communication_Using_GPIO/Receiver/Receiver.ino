#define send_enable 2
#define data 12
#define clk 11
bool previous_clk = LOW;
String receive_buffer;
int pointer = 0;
int bytes[8];
bool flag;


void setup() {

pinMode(send_enable, INPUT);
pinMode(data, INPUT);
pinMode(clk, INPUT);
Serial.begin(115200);
}

void loop() {
while (digitalRead(send_enable) == HIGH) {
  pinMode(clk, INPUT);
  flag = true;
  if (digitalRead(clk) == LOW){
    previous_clk = LOW;
    }
  if ((digitalRead(clk) == HIGH) && (previous_clk == LOW)){
        bytes[pointer] = digitalRead(data);
          int temp;
          if (pointer == 7) {
          pointer = -1;
          temp = 0;
          for (int j = 0; j < 8; j++) {
            int bitshiftedVal = (bytes[j] << (7-j));
            temp |= bitshiftedVal;           
              }
              
          receive_buffer.concat(char(temp));
           //or Serial.print(char(temp)) 
          } 
       pointer = pointer+1;
       previous_clk = HIGH;
       pinMode(clk, OUTPUT);
       digitalWrite(clk, LOW); 
       //delayMicroseconds(3);   
  }
    }
 pointer = 0;
 if (flag) { 
 Serial.println(receive_buffer);

  }

  
 flag = false; 
 receive_buffer = "";
 }
   
