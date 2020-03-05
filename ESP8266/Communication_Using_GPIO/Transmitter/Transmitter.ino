#define send_enable 15
#define data 12
#define clk 13
String receive_buffer = "Test String1 test string2 shit shit";
bool stringComplete = false;
bool previous_clk = HIGH;

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
pinMode (send_enable, OUTPUT);
pinMode (data, OUTPUT);
pinMode (clk, OUTPUT);
digitalWrite(send_enable, LOW);
}

void loop() {
  
      //receive_buffer = Serial.readString();  //read received data      
      //Serial.print("received data is: ");
      Serial.println(receive_buffer);        //display received data.
    
      digitalWrite(send_enable, HIGH);
      
     
      //transmit(receive_buffer);
      delayMicroseconds(1);

      tobinary(receive_buffer);

      delayMicroseconds(1);
      
      digitalWrite(send_enable, LOW);
      stringComplete = false;
      delay(100);
      
  }



void tobinary(String myText){
int bitno;
pinMode(clk, OUTPUT);
previous_clk = HIGH;
digitalWrite(clk, LOW);

for(int i=0; i<myText.length(); i++){
   char myChar = myText.charAt(i);
    
    bitno = 8;
    do {
    if (digitalRead(clk) == HIGH){
    previous_clk = HIGH;
    }
    if ((digitalRead(clk) == LOW) && (previous_clk == HIGH)){
      bitno--;
      pinMode(clk, OUTPUT);
      digitalWrite(data, bitRead(myChar, bitno));
      
      digitalWrite(clk, HIGH); 
      delayMicroseconds(50);      
      //digitalWrite(clk, LOW);
      //delayMicroseconds(1); 
      //Serial.print(bitRead(myChar, i));
      pinMode(clk, INPUT);
      delayMicroseconds(50); 
      
      }

      previous_clk = HIGH;
    } while (bitno != 0);
  } 

}
