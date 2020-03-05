#define send_enable 2
#define data 12
#define clk 11
String receive_buffer = "";
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
  // put your main code here, to run repeatedly:
  if(stringComplete)  //check for any data received
  {   
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
      
  }

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
      //delayMicroseconds(20);
      
      digitalWrite(clk, HIGH);       
      //digitalWrite(clk, LOW);
      //delayMicroseconds(1); 
      //Serial.print(bitRead(myChar, i));
      pinMode(clk, INPUT);
      
      }

      previous_clk = HIGH;
    } while (bitno != 0);
  } 

//Serial.println("");
receive_buffer = "";
}


void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    receive_buffer += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
