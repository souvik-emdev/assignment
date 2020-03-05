String receive_buffer;
#define led1 5
#define led2 4
#define led3 14
#define led4 16
String led1state;
String led2state;
String led3state;
String led4state;

void setup() {
  Serial.begin(9600);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
}

void loop() {
  if(Serial.available()>0)  //check for any data received
  {   
      receive_buffer = Serial.readString();  //read received data      
      Serial.print("received data is: ");
      Serial.println(receive_buffer);        //display received data

 Serial.print("led1:"); 
 Serial.println(receive_buffer.charAt(2));
 Serial.print("led2:"); 
 Serial.println(receive_buffer.charAt(6)); 
 Serial.print("led3:"); 
 Serial.println(receive_buffer.charAt(10));
 Serial.print("led4:"); 
 Serial.println(receive_buffer.charAt(14));
  }
 led1state = receive_buffer.charAt(2);
 led2state = receive_buffer.charAt(6);
 led3state = receive_buffer.charAt(10);
 led4state = receive_buffer.charAt(14);
 digitalWrite(led1, led1state.toInt());
 digitalWrite(led2, led2state.toInt());
 digitalWrite(led3, led3state.toInt());
 digitalWrite(led4, led4state.toInt());
  
}
