#define data_direction 2
#define data 12
#define clk 11
String receive_buffer;
bool flag;
void setup() {
  // put your setup code here, to run once:
  pinMode(clk, INPUT);
  pinMode(data_direction, INPUT);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
if (digitalRead(data_direction) == HIGH){
  receive_buffer = _receive();
  Serial.println(receive_buffer);
  }
}

String _receive(){
pinMode(data, INPUT);
receive_buffer = "";
bool previous_clk = LOW;
int pointer = 0;
int bytes[8];
while (digitalRead(data_direction) == HIGH) {
  flag = true;
  int pointer;
  if (digitalRead(clk) == LOW){
    previous_clk = LOW;
    }
  if ((digitalRead(clk) == HIGH) && (previous_clk == LOW)){
        bytes[pointer] = digitalRead(data);
          int temp;
          if (pointer == 7){
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
       //delayMicroseconds(3);   
       }
    }
   return receive_buffer;  
  }
