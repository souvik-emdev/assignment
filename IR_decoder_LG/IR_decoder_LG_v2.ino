/* Remote name: LG AC
 *  Format MSB
 */
//////////////////////////Setup////////////////////////////////

////power mode, on or off 
//bool power = 0;
////Temperature: Range is 16-30
//uint8_t temp = 16;
////Fan mode: Auto=4, High=3, Med=2, Low=1
//uint8_t fan = 4;
//bool hsw = 0;
//bool vsw = 0;
////Mode: Cool=1, Auto=2, Dry=3, Fan=4
//uint8_t mode = 1;
////The change from the previous, required for special blasts
////1-6
//uint8_t change = 1;


//////////////////////////////////////////////////////////////

//Sample array
//uint8_t ir_out[]={0x0,0x8,0x1,0x5}; //these positions are variable
uint8_t ir_out2[5]; //final decoded array which contain MSB

//Builds final ir_out2 taking fixed data and variable data from ir_out
//void build_ir_out2(){
//    ir_out2[0]=0x1C; //length to consider
//    ir_out2[1]=0x88;
//    uint8_t i=0;
//    uint8_t val=0,j=2;
//    for (i=0; i<4; i++){
//        val = (ir_out[i]<<4);
//        val= val|ir_out[i+1];
//        ir_out2[j]=val;
//        i++;
//        val =0;
//        j++;
//    }
//}
////////////////////////////////////////////////////////////////

void setpower(bool power, uint8_t temp, uint8_t fan, uint8_t mode){
 if (power){
  settemp(temp);
  setfan(fan);
  if (mode == 1){ir_out2[2]=0x0;}
  else if (mode == 2){ir_out2[2]=0x03;}
  else if (mode == 3){ir_out2[2]=0x01;}
  else if (mode == 4){ir_out2[2]=0x02;}
  }
  else {ir_out2[2]=0xC0;ir_out2[3]=0x05;}
}

void setswing(uint8_t change, bool hsw, bool vsw){
  if (change == 4){
    if (hsw){ir_out2[2]=0x13;ir_out2[3]=0x16;}
    else {ir_out2[2]=0x13;ir_out2[3]=0x17;}
  }
  else if (change == 5){
    if (vsw){ir_out2[2]=0x13;ir_out2[3]=0x14;}
    else {ir_out2[2]=0x13;ir_out2[3]=0x15;}
  }
}

void setfan(uint8_t fan){
  ir_out2[3]=(ir_out2[3] & 0xF0);
  if (fan==1){ir_out2[3]=(ir_out2[3] | 0x00);}
  else if (fan==2){ir_out2[3]=ir_out2[3] | 0x02;}
  else if (fan==3){ir_out2[3]=ir_out2[3] | 0x04;} 
  else if (fan==4){ir_out2[3]=ir_out2[3] | 0x05;}

}

void settemp(uint8_t temp){
  ir_out2[3]=(ir_out2[3] & 0x0F);
  uint8_t val =0x10;
  for(int i=16; i<=30; i++){   
    if (i==temp){break;}
    val += 10;
    }
  ir_out2[3]=val | ir_out2[3];
}

void setmode(uint8_t mode){
  if (mode == 1){ir_out2[2]=0x08;} //cool
  else if (mode == 2){ir_out2[2]=0x0B;settemp(17);setfan(4);} //auto, set temp & fan
  else if (mode == 3){ir_out2[2]=0x09;} //dry
  else if (mode == 4){ir_out2[2]=0x0A;settemp(18);} //fan, set temp
}


void print_ir_out2(){
  Serial.println("");
  Serial.println("Decoded remote signal is:");
  for (int i=0; i<5; i++){
    Serial.print(ir_out2[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");
}

void summation(){
byte sum = 0;
ir_out2[4] = 0;
for(int k=1;k<4;k++){
     // Serial.println(k);
     byte nibble1 = (byte) (ir_out2[k] & 0x0F);
     byte nibble2 = (byte)((ir_out2[k] & 0xF0) >> 4);
     sum += (nibble1 + nibble2);
   }
   ir_out2[4] = (byte)((sum & 0x0F)<<4);
//   Serial.println("");
//   Serial.print("CheckSum:");
//   Serial.println(checksum, HEX);  
}

void driver(bool power, uint8_t temp, uint8_t fan, bool hsw, bool vsw, uint8_t mode, uint8_t change){

  ir_out2[0]=0x1C; //length to consider
  ir_out2[1]=0x88;
  
  if (change == 1){
    setpower(power, temp, fan, mode);
  }
  else if ((change == 4) || (change == 5)){
    setswing(change, hsw, vsw);
  }
  else{
    settemp(temp);
    setfan(fan);
    setmode(mode);
  }
    
 // build_ir_out2();

  summation(); //checksum generation
  
}

void setup() {
  Serial.begin(115200);
  
//  ***INFO***
//  Format: power[1/0],temp[16-30],fan[1-4],hsw[1/0],vsw[1/0],mode[1-4],change[1-6]
//  Fan mode: Auto=4, High=3, Med=2, Low=1
//  Mode: Cool=1, Auto=2, Dry=3, Fan=4
  
  driver(1,16,4,0,0,1,2);
  
  print_ir_out2();
  
}


void loop() {

}
