#define data_direction 2
#define data 12
#define clk 11
String receive_buffer;
//bool flag;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(data_direction, INPUT); 
}

void loop() {
  // put your main code here, to run repeatedly:
if (digitalRead(data_direction) == HIGH){
  receive_buffer = _receive();
  //delay(50);
  transmit("Hello from Arduino");
  Serial.println(receive_buffer);
  
  }
 
}

String _receive(){
pinMode(data, INPUT);
pinMode(data_direction, INPUT);
pinMode(clk, INPUT);
receive_buffer = "";
bool previous_clk = LOW;
int pointer = 0;
int bytes[8];
while (digitalRead(data_direction) == HIGH) {
//  flag = true;
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

void transmit(String myText){
pinMode(data, OUTPUT);
pinMode(clk, OUTPUT);
pinMode(data_direction, OUTPUT);
digitalWrite(data_direction, HIGH);
delay(1);
for(int i=0; i<myText.length(); i++){
   char myChar = myText.charAt(i);
 
    for(int i=7; i>=0; i--){
      
      digitalWrite(data, bitRead(myChar, i));
      digitalWrite(clk, HIGH); 
      //delayMicroseconds(10);
      digitalWrite(clk, LOW);
      //delayMicroseconds(50); 
      //Serial.print(bitRead(myChar, i));
      }
  }
//receive_buffer = "";
digitalWrite(data_direction, LOW);
pinMode(data_direction, INPUT);
}
