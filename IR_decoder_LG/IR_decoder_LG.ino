/* Remote name: LG AC
 *  Format MSB
 */
//////////////////////////Setup////////////////////////////////

//Mode: Cool=1, Auto=2, Dry=3, Fan=4, Off=5
uint8_t mode = 1;
//Temperature: Range is 16-30
uint8_t temp = 16;
//Fan mode: Auto=1, High=2, Med=3, Low=4
uint8_t fan = 1;



//////////////////////////////////////////////////////////////

String inputString = "";      // a String to hold incoming data from serial
bool stringComplete = false;  // whether the serial string is complete
char received_data[15]; //string to char for better effeciency
uint8_t p_m = 0; //previous mode, required to generate 'ON' mode bits

//Sample array
uint8_t ir_out[]={0x0,0x8,0x1,0x5,0x0}; //these positions are variable
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
  if (mode == 1){ir_out[0]=0x0;ir_out[1]=0x8;p_m=1;} //cool
  else if (mode == 2){ir_out[0]=0x0;ir_out[1]=0xB;ir_out[2]=0x2;ir_out[3]=0x5;p_m=2;} //auto, set temp & fan
  else if (mode == 3){ir_out[0]=0x0;ir_out[1]=0x9;p_m=3;} //dry
  else if (mode == 4){ir_out[0]=0x0;ir_out[1]=0xA;ir_out[2]=0x3;p_m=4;} //fan, set temp
  else if (mode == 5){                                     //On. Depends on the previous mode
    ir_out[0]=0x0;
    if (p_m == 1){ir_out[1]=0x0;}
    else if (p_m == 2){ir_out[1]=0x3;}
    else if (p_m == 3){ir_out[1]=0x1;}
    else if (p_m == 4){ir_out[1]=0x2;}
  }
  else if (mode == 6){ir_out[0]=0x1;ir_out[1]=0x3;ir_out[2]=0x0;ir_out[3]=0xB;} //hsw on
  else if (mode == 7){ir_out[0]=0x1;ir_out[1]=0x3;ir_out[2]=0x1;ir_out[3]=0x7;} //hsw off
  else if (mode == 8){ir_out[0]=0x1;ir_out[1]=0x3;ir_out[2]=0x1;ir_out[3]=0x4;} //vsw on
  else if (mode == 9){ir_out[0]=0x1;ir_out[1]=0x3;ir_out[2]=0x1;ir_out[3]=0x5;} //vsw off
  else if (mode == 10){ir_out[0]=0xC;ir_out[1]=0x0;ir_out[2]=0x0;ir_out[3]=0x5;} //OFF
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

void setup() {
  Serial.begin(115200);
  Serial.println("Submit data in the following format to decode:");
  Serial.println("Mode[1-10],Temperature[16-30],FanMode[1-4]");
}


void loop() {
if(stringComplete){
  stringComplete = false;
  inputString.toCharArray(received_data, 15);
  inputString = "";
  parse_data(received_data); //set setup variables
  if (mode < 5){
    settemp();
    setfan();
    setmode(); 
  }
  else setmode();
  
  //CheckSum generation here
    
  build_ir_out2();
  Serial.print(": MSB Format :");
  print_ir_out2();
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
