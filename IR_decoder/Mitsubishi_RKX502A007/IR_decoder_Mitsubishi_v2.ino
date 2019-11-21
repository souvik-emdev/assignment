/* Remote name: Mitsubishi AC
 *  Format LSB8 to MSB
 */
//////////////////////////Setup////////////////////////////////

////Mode: Fan=1, Cool=2, Heat=3, Humidity=4, Off=5
//uint8_t mode = 0;
////Temperature: Range is 18-30
//uint8_t temp = 15;
////Fan mode: Auto=1, High=2, Med=3, Low=4
//uint8_t fan = 0;
////Horizontal swing
//bool hsw = 0;
////vertical swing
//bool vsw = 0;

//////////////////////////////////////////////////////////////

//Sample array
//uint8_t ir_out[]={0xF,0xF,0x0,0x0,0xF,0xF,0x0,0x0,0xA,0x6,0x5,0x9}; //these positions are variable
uint8_t ir_out2[11]; //final decoded array which contain both LSB8 and MSB
uint8_t p_m = 0;     //previous mode, required to generate 'OFF' mode bits

//LSB8 to MSB converter
void lsb8_to_msb()
{
  uint8_t reverse_num = 0;
  uint8_t val = 0;
  for (uint8_t j = 0; j < 11; j++)
  {
    for (uint8_t i = 0; i < 8; i++)
    {
      val = (ir_out2[j] & (1 << i));
      if (val)
        reverse_num |= (1 << ((8 - 1) - i));
    }
    ir_out2[j] = reverse_num;
    val = 0;
    reverse_num = 0;
  }
}

//Builds final ir_out2 taking fixed data and variable data from ir_out
//void build_ir_out2(){
//    ir_out2[0]=0x52;
//    ir_out2[1]=0xAE;
//    ir_out2[2]=0xC3;
//    ir_out2[3]=0x26;
//    ir_out2[4]=0xD9;
//    uint8_t i=0;
//    uint8_t val=0,j=5;
//    for (i=0; i<12; i++){
//        val = (ir_out[i]<<4);
//        val= val|ir_out[i+1];
//        ir_out2[j]=val;
//        i++;
//        val =0;
//        j++;
//    }
//}

void setswing(bool hsw, bool vsw)
{
  if (hsw)
  {
    ir_out2[5] = 0x7F;
    ir_out2[6] = 0x80;
  }
  else
  {
    ir_out2[5] = 0xFF;
    ir_out2[6] = 0x00;
  }
  if (vsw)
  {
    ir_out2[7] -= 0x10;
    ir_out2[8] += 0x10;
  }
}

void setfan(uint8_t fan)
{
  ir_out2[7] = 0x0F;
  ir_out2[8] = 0x00;
  if (fan == 1)
  {
    ir_out2[7] |= 0xF0;
    ir_out2[8] |= 0x0;
  }
  else if (fan == 2)
  {
    ir_out2[7] |= 0x70;
    ir_out2[8] |= 0x80;
  }
  else if (fan == 3)
  {
    ir_out2[7] |= 0x90;
    ir_out2[8] |= 0x60;
  }
  else if (fan == 4)
  {
    ir_out2[7] |= 0xB0;
    ir_out2[8] |= 0x40;
  }
}

void settemp(uint8_t temp)
{
  ir_out2[9] = (ir_out2[9] & 0x0F);
  ir_out2[10] = (ir_out2[10] & 0x0F);
  ir_out2[9] = (ir_out2[9] | 0xE0);
  ir_out2[10] = (ir_out2[10] | 0x10);

  for (int i = 18; i <= 30; i++)
  {
    if (i == temp)
    {
      break;
    }
    ir_out2[9] -= 0x10;
    ir_out2[10] += 0x10;
  }
}

void setmode(uint8_t mode)
{
  ir_out2[9] = (ir_out2[9] & 0xF0);
  ir_out2[10] = (ir_out2[10] & 0xF0);

  if (mode == 1)
  {
    ir_out2[9] = 0x27;
    ir_out2[10] = 0xD8;
    p_m = 1;
  } //fan
  else if (mode == 2)
  {
    ir_out2[9] |= 0x06;
    ir_out2[10] |= 0x09;
    p_m = 2;
  } //cool
  else if (mode == 3)
  {
    ir_out2[9] |= 0x03;
    ir_out2[10] |= 0x0C;
    p_m = 3;
  } //heat
  else if (mode == 4)
  {
    ir_out2[9] |= 0x05;
    ir_out2[10] |= 0x0A;
    p_m = 4;
  } //dry
  else if (mode == 5)
  { //Off. Depends on the previous mode
    if (p_m == 1)
    {
      ir_out2[9] |= 0x0F;
      ir_out2[10] |= 0x00;
    }
    else if (p_m == 2)
    {
      ir_out2[9] |= 0x0E;
      ir_out2[10] |= 0x01;
    }
    else if (p_m == 3)
    {
      ir_out2[9] |= 0x0B;
      ir_out2[11] |= 0x04;
    }
    else if (p_m == 4)
    {
      ir_out2[9] |= 0x0D;
      ir_out2[11] |= 0x02;
    }
  }
}

void print_ir_out2()
{
  Serial.println("");
  Serial.println("Decoded remote signal is:");
  for (int i = 0; i < 11; i++)
  {
    Serial.print(ir_out2[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");
}

void ir_generator(uint8_t temp, uint8_t fan, bool hsw, bool vsw, uint8_t mode)
{
  ir_out2[0] = 0x52;
  ir_out2[1] = 0xAE;
  ir_out2[2] = 0xC3;
  ir_out2[3] = 0x26;
  ir_out2[4] = 0xD9;
  settemp(temp);
  setfan(fan);
  setswing(hsw, vsw);
  setmode(mode);
  lsb8_to_msb();
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Submit data in the following format to decode:");
  Serial.println("Temperature[18-30],FanMode[1-4],HSW[0/1],VSW[0/1],Mode[1-5]");
  //Mode: Fan=1, Cool=2, Heat=3, Humidity=4, Off=5
  //Fan mode: Auto=1, High=2, Med=3, Low=4

  //  pinMode(ir_pin, OUTPUT);

  ir_generator(21, 2, 0, 1, 4);
  print_ir_out2();
}

void loop()
{
  //if(stringComplete){
  //  stringComplete = false;
  //  inputString.toCharArray(received_data, 15);
  //  inputString = "";
  //  parse_data(received_data); //set setup variables
  //  settemp();
  //  setfan();
  //  setswing();
  //  setmode();
  //  build_ir_out2();
  //  Serial.print(": LSB8 Format :");
  //  print_ir_out2();
  //  lsb8_to_msb();
  //  Serial.print(": MSB Format :");
  //  print_ir_out2();
  //  }
}
