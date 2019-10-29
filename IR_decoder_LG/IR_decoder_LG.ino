/* Remote name: LG AC
 *  Format MSB
 */
//////////////////////////Setup////////////////////////////////

//power mode, on or off 
bool power = 0;
//Temperature: Range is 16-30
uint8_t temp = 16;
//Fan mode: Auto=4, High=3, Med=2, Low=1
uint8_t fan = 4;
bool hsw = 0;
bool vsw = 0;
//Mode: Cool=1, Auto=2, Dry=3, Fan=4
uint8_t mode = 1;
//The change from the previous, required for special blasts
//1-6
uint8_t change = 1;


//////////////////////////////////////////////////////////////

String inputString = "";      // a String to hold incoming data from serial
bool stringComplete = false;  // whether the serial string is complete
char received_data[15]; //string to char for better effeciency

//Sample array
uint8_t ir_out[]={0x0,0x8,0x1,0x5}; //these positions are variable
uint8_t ir_out2[4]; //final decoded array which contain MSB

//Builds final ir_out2 taking fixed data and variable data from ir_out
void build_ir_out2(){
    ir_out2[0]=0x88;
    uint8_t i=0;
    uint8_t val=0,j=1;
    for (i=0; i<4; i++){
        val = (ir_out[i]<<4);
        val= val|ir_out[i+1];
        ir_out2[j]=val;
        i++;
        val =0;
        j++;
    }
}
////////////////////////////////////////////////////////////////

void setpower(){
 if (power){
  ir_out[0]=0x0;
  settemp();
  setfan();
  if (mode == 1){ir_out[1]=0x0;}
  else if (mode == 2){ir_out[1]=0x3;}
  else if (mode == 3){ir_out[1]=0x1;}
  else if (mode == 4){ir_out[1]=0x2;}
  }
  else {ir_out[0]=0xC;ir_out[1]=0x0;ir_out[2]=0x0;ir_out[3]=0x5;}
}

void setswing(){
  if (change == 4){
    if (hsw){ir_out[0]=0x1;ir_out[1]=0x3;ir_out[2]=0x1;ir_out[3]=0x6;}
    else {ir_out[0]=0x1;ir_out[1]=0x3;ir_out[2]=0x1;ir_out[3]=0x7;}
  }
  else if (change == 5){
    if (vsw){ir_out[0]=0x1;ir_out[1]=0x3;ir_out[2]=0x1;ir_out[3]=0x4;}
    else {ir_out[0]=0x1;ir_out[1]=0x3;ir_out[2]=0x1;ir_out[3]=0x5;}
  }
}

void setfan(){
  if (fan==1){ir_out[3]=0x0;}
  else if (fan==2){ir_out[3]=0x2;}
  else if (fan==3){ir_out[3]=0x4;} 
  else if (fan==4){ir_out[3]=0x5;}
}

void settemp(){
  ir_out[2]=0x1;
  for(int i=16; i<=30; i++){   
    if (i==temp){break;}
    ir_out[2]+=1;
    }
}

void setmode(){
  if (mode == 1){ir_out[0]=0x0;ir_out[1]=0x8;} //cool
  else if (mode == 2){ir_out[0]=0x0;ir_out[1]=0xB;ir_out[2]=0x2;ir_out[3]=0x5;} //auto, set temp & fan
  else if (mode == 3){ir_out[0]=0x0;ir_out[1]=0x9;} //dry
  else if (mode == 4){ir_out[0]=0x0;ir_out[1]=0xA;ir_out[2]=0x3;} //fan, set temp
}

void parse_data(char received[]){ //Parsing the incoming serial data and store in setup variables
  String token = "";
  char *tok = strtok(received, ",");
  int i = 0;
  while (tok != NULL) {
     if (i == 0){
      token = tok; 
      power = token.toInt();
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
     if (i == 5){
     token = tok;
     mode = token.toInt();
     }
     if (i == 6){
     token = tok;
     change = token.toInt();
     }
     tok = strtok(NULL, ",");
     i++;
  }  
}

void print_ir_out2(){
  Serial.println("");
  Serial.println("Decoded remote signal is:");
  for (int i=0; i<4; i++){
    Serial.print(ir_out2[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");
}

void summation(){
byte sum = 0;
ir_out2[3] = 0;
for(int k=0;k<4;k++){
     // Serial.println(k);
     byte nibble1 = (byte) (ir_out2[k] & 0x0F);
     byte nibble2 = (byte)((ir_out2[k] & 0xF0) >> 4);
     sum += (nibble1 + nibble2);
   }
   ir_out2[3] = (byte)(sum & 0x0F);
//   Serial.println("");
//   Serial.print("CheckSum:");
//   Serial.println(checksum, HEX);  
}

void print_help(){
  Serial.println("Submit data in the following format to decode:");
  Serial.println("Power,Temperature[16-30],FanMode[1-4],HSW,VSW,Mode,Change");
  Serial.println("Mode:1-Cool,2-Auto,3-Dry,4-Fan");
  Serial.println("Fan:1-Low,2-Med,3-High,4-Auto");  
}

void setup() {
  Serial.begin(115200);
  print_help();
}


void loop() {
if(stringComplete){
  stringComplete = false;
  inputString.toCharArray(received_data, 15);
  inputString = "";
  parse_data(received_data); //set setup variables
  
  if (change == 1){
    setpower();
  }
  else if ((change == 4) || (change == 5)){
    setswing();
  }
  else{
    settemp();
    setfan();
    setmode();
  }
    
  build_ir_out2();

  summation(); //checksum generation
  
  Serial.print(": MSB Format :");
  print_ir_out2();
  print_help();
  Serial.println(""); 
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
