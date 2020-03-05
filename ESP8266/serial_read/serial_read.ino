String receive_buffer;
void setup() {
  Serial.begin(9600);
}

void loop() {
  if(Serial.available()>0)  //check for any data received
  {   
      receive_buffer = Serial.readString();  //read received data      
      Serial.print("received data is: ");
      Serial.println(receive_buffer);        //display received data
  }
}
