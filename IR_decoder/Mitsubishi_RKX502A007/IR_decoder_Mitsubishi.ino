/* Remote name: Mitsubishi AC
 *  Format LSB8 to MSB
 */
#define ir_pin 8
//////////////////////////Setup////////////////////////////////

//Mode: Fan=1, Cool=2, Heat=3, Humidity=4, Off=5
uint8_t mode = 0;
//Temperature: Range is 18-30
uint8_t temp = 15;
//Fan mode: Auto=1, High=2, Med=3, Low=4
uint8_t fan = 0;
//Horizontal swing
bool hsw = 0;
//vertical swing
bool vsw = 0;

//////////////////////////////////////////////////////////////

String inputString = "";      // a String to hold incoming data from serial
bool stringComplete = false;  // whether the serial string is complete
char received_data[15]; //string to char for better effeciency
uint8_t p_m = 0; //previous mode, required to generate 'OFF' mode bits

//Sample array
uint8_t ir_out[]={0xF,0xF,0x0,0x0,0xF,0xF,0x0,0x0,0xA,0x6,0x5,0x9}; //these positions are variable
uint8_t ir_out2[11]; //final decoded array which contain both LSB8 and MSB

//LSB8 to MSB converter
void lsb8_to_msb(){
uint8_t reverse_num = 0;
uint8_t val = 0;
 for (uint8_t j=0; j<11; j++){
  for (uint8_t i=0; i<8; i++){
    val = (ir_out2[j] & (1 << i));
    if(val)
    reverse_num |= (1 << ((8 - 1) - i));
  }
  ir_out2[j] = reverse_num;
  val = 0;
  reverse_num = 0;
 }  
}

//Builds final ir_out2 taking fixed data and variable data from ir_out
void build_ir_out2(){
    ir_out2[0]=0x52;
    ir_out2[1]=0xAE;
    ir_out2[2]=0xC3;
    ir_out2[3]=0x26;
    ir_out2[4]=0xD9;
    uint8_t i=0;
    uint8_t val=0,j=5;
    for (i=0; i<12; i++){
        val = (ir_out[i]<<4);
        val= val|ir_out[i+1];
        ir_out2[j]=val;
        i++;
        val =0;
        j++;
    }
}


void setswing(){
  if (hsw){ir_out[0]=0x7;ir_out[1]=0xF;ir_out[2]=0x8;ir_out[3]=0x0;}
  else {ir_out[0]=0xF;ir_out[1]=0xF;ir_out[2]=0x0;ir_out[3]=0x0;}
  if (vsw){ir_out[4]-=1;ir_out[6]+=1;}
}

void setfan(){
  ir_out[5]=0xF;
  ir_out[7]=0x0;
  if (fan==1){ir_out[4]=0xF;ir_out[6]=0x0;}
  else if (fan==2){ir_out[4]=0x7;ir_out[6]=0x8;}
  else if (fan==3){ir_out[4]=0x9;ir_out[6]=0x6;} 
  else if (fan==4){ir_out[4]=0xB;ir_out[6]=0x4;}
}

void settemp(){
  ir_out[8]=0xE;
  ir_out[10]=0x1;
  for(int i=18; i<=30; i++){   
    if (i==temp){break;}
    ir_out[8]-=1;
    ir_out[10]+=1;
    }
}

void setmode(){
  if (mode == 1){ir_out[8]=0x2;ir_out[9]=0x7;ir_out[10]=0xD;ir_out[11]=0x8;p_m=1;} //fan
  else if (mode == 2){ir_out[9]=0x6;ir_out[11]=0x9;p_m=2;} //cool
  else if (mode == 3){ir_out[9]=0x3;ir_out[11]=0xC;p_m=3;} //heat
  else if (mode == 4){ir_out[9]=0x5;ir_out[11]=0xA;p_m=4;} //humid
  else if (mode == 5){                                     //Off. Depends on the previous mode
    if (p_m == 1){ir_out[9]=0xF;ir_out[11]=0x0;}
    else if (p_m == 2){ir_out[9]=0xE;ir_out[11]=0x1;}
    else if (p_m == 3){ir_out[9]=0xB;ir_out[11]=0x4;}
    else if (p_m == 4){ir_out[9]=0xD;ir_out[11]=0x2;}
}
}

void parse_data(char received[]){ //Parsing the incoming serial data and store in setup variables
  String token = "";
  char *tok = strtok(received, ",");
  int i = 0;
  while (tok != NULL) {
     if (i == 0){
      token = tok;
      mode = token.toInt();      
      }
     if (i == 1){
     token = tok;
     temp = token.toInt();
     }
     if (i == 2){
     token = tok;
     fan = token.toInt();
     }
     if (i == 3){
     token = tok;
     hsw = token.toInt();
     }
     if (i == 4){
     token = tok;
     vsw = token.toInt();
     }
     tok = strtok(NULL, ",");
     i++;
  }  
}

void print_ir_out2(){
  Serial.println("");
  Serial.println("Decoded remote signal is:");
  for (int i=0; i<11; i++){
    Serial.print(ir_out2[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");
}


void transmit(){
  startbit();
  bool bitvalue = 0;
  //Serial.println("bitvalue");
  for (int i=0; i<11; i++){
    for (int j=7; j>=0; j--){
      bitvalue = bitRead(ir_out2[i], j);
      //Serial.print(bitvalue); 
      if(bitvalue){tx1();}
      else {tx0();}     
    }
    //Serial.print(" ");
  }
  stopbit();
}

void tx1(){
  digitalWrite(ir_pin, HIGH);
  delayMicroseconds(394);
  digitalWrite(ir_pin, LOW);
  delayMicroseconds(1180);  
}

void tx0(){
  digitalWrite(ir_pin, HIGH);
  delayMicroseconds(406);
  digitalWrite(ir_pin, LOW);
  delayMicroseconds(382);  
}

void startbit(){
  digitalWrite(ir_pin, HIGH);
  delayMicroseconds(3168);
  digitalWrite(ir_pin, LOW);
  delayMicroseconds(568);
}

void stopbit(){
  digitalWrite(ir_pin, HIGH);
  delayMicroseconds(406);
  digitalWrite(ir_pin, LOW);  
}

void setup() {
  Serial.begin(115200);
  Serial.println("Submit data in the following format to decode:");
  Serial.println("Mode[1-5],Temperature[18-30],FanMode[1-4],HSW[0/1],VSW[0/1]");
  pinMode(ir_pin, OUTPUT);
}


void loop() {
if(stringComplete){
  stringComplete = false;
  inputString.toCharArray(received_data, 15);
  inputString = "";
  parse_data(received_data); //set setup variables
  settemp();
  setfan();
  setswing();
  setmode();
  build_ir_out2();
  Serial.print(": LSB8 Format :");
  print_ir_out2();
  lsb8_to_msb();
  Serial.print(": MSB Format :");
  print_ir_out2();
  Serial.println("");
  transmit();
  }
}

//Called whenever serial data is present
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
