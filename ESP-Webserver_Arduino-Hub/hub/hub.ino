#define data_direction 2
#define data 12
#define clk 11
#include <EEPROM.h>
int match = 11;
int eeAddress = 0;
int eename = 800;
String receive_buffer;
String scnname[] = {"", "", "", "", "", "", "", "", "", ""};
char received_data[75];
//String printdata; //debug print
//bool sceane_count[10];
//bool flag;


//////////////////////////////////////////////////////////////
//EEPROM Read and write
/////////////////////////////////////////////////////////////
void eepromwrite(int address, String info){
  int size1 = info.length();
  for (int i=0; i<size1; i++){
    EEPROM.write(address+i, info[i]);
    }
  EEPROM.write(address+size1, '\0');
  }

String eepromread(int address){
  int i;
  char read_[100];
  int len = 0;
  unsigned char k;
  k=EEPROM.read(address);
  while(k != '\0' && len<500){
    k = EEPROM.read(address+len);
    read_[len] = k;
    len++;
    }
  read_[len] = '\0';
  return String(read_);
  }

//////////////////////////////////////////////////////////////
//Data Receiver
/////////////////////////////////////////////////////////////

void _receive(){

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
   //return receive_buffer;  
  }
/////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
//Data Transmitter
/////////////////////////////////////////////////////////////

void transmit(String myText){
//delay(1);
pinMode(data, OUTPUT);
pinMode(clk, OUTPUT);
pinMode(data_direction, OUTPUT);

digitalWrite(data_direction, HIGH);
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
digitalWrite(data_direction, LOW);
pinMode(data_direction, INPUT);
pinMode(data, INPUT);
pinMode(clk, INPUT);
}
/////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////
//Sceane delete function
/////////////////////////////////////////////////////////////
void Del(String scninfo){
  if (match < 10){
      eeAddress = 0;
      eename = 800;
      for (int i=0; i<=9; i++){
        if (i == match){
          //send feedback message
          transmit("Deleted");
          //do deletation
          for (int k=0; k<(sizeof(eepromread(eeAddress))); k++){
             EEPROM.write(eeAddress+k, 0);
             }
          //do name array update
          
          for (int j=0; j<sizeof(scnname[i]); j++){
             EEPROM.write(eename+j, 0);
             }
          scnname[i] = "";   
          //eepromwrite(eename, scnname[i]);
          
          break;
        }
      eeAddress += 71;
      eename += 21;      
      } 
    }
   else
   transmit("Sceane not found");
  }

//////////////////////////////////////////////////////////////
//Sceane info function
/////////////////////////////////////////////////////////////  
String Info(String scninfo){
  if (match < 10){
      eeAddress = 0;
      for (int i=0; i<=9; i++){
        if (i == match){
          return(eepromread(eeAddress));
          break;
        }
      eeAddress += 71;      
      } 
    }
  else if (match > 9)
   transmit("Sceane name not found");
   else transmit(match);
  }
  

//////////////////////////////////////////////////////////////
//Sceane add function
/////////////////////////////////////////////////////////////
void Sadd(String scninfo){
  //String printdata; // debug print
  char scndata[71];
  memset(scndata, 0, sizeof(scndata));
  scninfo.toCharArray(scndata, 71);
  char *token = strtok(scndata, ",");
  String SCNName = token;
  eeAddress = 0;
  eename = 800;
  namecheck(SCNName);
  if (match > 9){
  for (int i=0; i<=9; i++){
    
    if (scnname[i] == "" ){
      transmit("Sceane Added");      
      scnname[i] = SCNName;
      eepromwrite(eeAddress, scninfo);
      eepromwrite(eename, scnname[i]);
//      Serial.println(eename);
//      Serial.println(scnname[i]);
      break;
      }
    eeAddress += 71;
    eename += 21;

      if (i==9)
      transmit("Memory full");
    }
    
  }
  if (match < 9)
  transmit("Sceane Already exists");
  SCNName = "";
  }

//////////////////////////////////////////////////////////////
//Name-check function
/////////////////////////////////////////////////////////////

void namecheck(String scninfo){
  match = 10;
  char scndata[71];
  memset(scndata, 0, sizeof(scndata));
  scninfo.toCharArray(scndata, 71);
  char *token = strtok(scndata, ",");
  String SCNName2 = token;
//  Serial.print("SCNName2:");
//  Serial.println(SCNName2);
  for (int i=0; i<10; i++){
    if (scnname[i] == SCNName2){
      match = i;
      break;
      }
    }
  SCNName2 = "";
  }
//////////////////////////////////////////////////////////////
//FindDev Function
/////////////////////////////////////////////////////////////  
void FindDev(String input_data, String scninfo){
  char device_name[27];
  char scndata[71];
  memset(scndata, 0, sizeof(scndata));
  memset(device_name, 0, sizeof(device_name));
  input_data.toCharArray(device_name, 27);
  scninfo.toCharArray(scndata, 71);
  char *token2 = strtok(device_name, ",");
  token2 = strtok(NULL, ",");
  //token2 = strtok(NULL, device_name);
  String device = token2;
  char *token = strtok(scndata, ",");
  //Serial.println(device);
  int pointer = 0;
  int counter = 0;
  bool flag = false;
  while (token != NULL){
    if ((device == "dev01") && ((pointer == 3) || (pointer == 4) ||(pointer == 5) ||(pointer == 6))){
      counter++;   
      String switchstate = token;
      if (switchstate.toInt() > 0) {
        transmit("Device included");
        flag = true;
        break;
        }
      if ((counter == 4) && (flag == false))
      transmit("Device not included");      
    } 

      if ((device == "dev02") && ((pointer == 8) || (pointer == 9) ||(pointer == 10) ||(pointer == 11))){
       counter++;
      String switchstate = token;
      if (switchstate.toInt() > 0 ) {
        transmit("Device included");
        flag = true;
        break;
        }
      if ((counter == 4) && (flag == false))
      transmit("Device not included");
    } 
      
    token = strtok(NULL, ",");
    pointer++; 
      
    }   
  }

//////////////////////////////////////////////////////////////
//FindDevInfo Function
/////////////////////////////////////////////////////////////

void FindDevInfo(String input_data, String scninfo){
  char device_name[27];
  char scndata[71];
  memset(scndata, 0, sizeof(scndata));
  memset(device_name, 0, sizeof(device_name));
  input_data.toCharArray(device_name, 27);
  scninfo.toCharArray(scndata, 71);
  char *token2 = strtok(device_name, ",");
  token2 = strtok(NULL, ",");
  //token2 = strtok(NULL, device_name);
  String device = token2;
  char *token = strtok(scndata, ",");
  //Serial.println(device);
  int pointer = 0;
  String switchinfo = "";
  while (token != NULL){
    if ((device == "dev01") && ((pointer == 3) || (pointer == 4) ||(pointer == 5) ||(pointer == 6))){  
      String switchstate = token;
      switchinfo.concat(switchstate);
      switchinfo.concat(",");  
    } 
    if ((device == "dev02") && ((pointer == 8) || (pointer == 9) ||(pointer == 10) ||(pointer == 11))){  
      String switchstate = token;
      switchinfo.concat(switchstate);
      switchinfo.concat(",");  
    }  
      
    token = strtok(NULL, ",");
    pointer++; 
      
    } 
    transmit(switchinfo);  
  }


//////////////////////////////////////////////////////////////
//Received data parsing
/////////////////////////////////////////////////////////////

void parse_data(char received[]){
    char *token = strtok(received, "-");
    String function = token;
    token = strtok(NULL, "-");
    String RestData = token;
    
    if (function == "Sadd"){
      Sadd(RestData);     
      }
    else if (function == "Info"){
      Serial.println("Info running");
      namecheck(RestData);
      String scninfo96 = Info(RestData);
      transmit(scninfo96);
      Serial.println(scninfo96);
      }
    else if (function == "Del"){
      namecheck(RestData);
      Del(RestData);
      }
    else if (function == "FindDev"){
      namecheck(RestData);
      String scninfo35 = Info(RestData);
      FindDev(RestData, scninfo35);     
      }
    else if (function == "FindDevInfo"){
      namecheck(RestData);
      String scninfo36 = Info(RestData);
      FindDevInfo(RestData, scninfo36);
      }
     else
     transmit("Function not found");
      
    RestData = "";
  }


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(data_direction, INPUT); 
  for (int i=0; i<=9; i++){
      //printdata = eepromread(eeAddress);
      //EEPROM.get(eename, scnname[i]);
      scnname[i] = eepromread(eename);
      Serial.print("Sceane");
      Serial.println(i);
      Serial.println(eepromread(eeAddress));
      eeAddress += 71; 
      eename += 21;
      Serial.println(scnname[i]);     
    }        
}

void loop() {
  // put your main code here, to run repeatedly:
if (digitalRead(data_direction) == HIGH){
  Serial.println("Reading");
  _receive();
  receive_buffer.toCharArray(received_data, 75);
  parse_data(received_data);
  //delay(50);   
  } 
}
