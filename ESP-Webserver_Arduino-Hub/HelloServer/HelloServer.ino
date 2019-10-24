#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#define data_direction 15
#define data 12
#define clk 13
String receive_buffer;

const char* ssid = "Vikilabs";
const char* password = "quicksilver";

ESP8266WebServer server(80);

const int led = 5;
const int led2 = 4;

void handleRoot() {
  digitalWrite(led, 1);
  
  transmit(server.arg(0));
  digitalWrite(led2, 1);
  //Serial.println(server.arg(0));
  receive_buffer = receiver();
  digitalWrite(led2, 0);
  server.send(200, "text/plain", receive_buffer);
  digitalWrite(led, 0);
}

void getSceneInfo() {
  uint8 totalArgs = server.args();

  for (uint8 i = 0; i < totalArgs; i++) {
    Serial.print(server.argName(i));
    Serial.print(" : ");
    Serial.print(server.arg(i));
    Serial.println(" ");
  }
  Serial.println(server.arg("sw"));
  digitalWrite(led, server.arg("sw").toInt());
  server.send(200, "text/plain", "received scene info, thanks.");
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(data_direction, OUTPUT);
  digitalWrite(led, 0);
  digitalWrite(led2, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(""); 
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/getsceneinfo", getSceneInfo);
  
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

String receiver(){
bool flag = false;
//pinMode(data, INPUT);
//pinMode(data_direction, INPUT);
//pinMode(clk, INPUT);
receive_buffer = "";
bool previous_clk = LOW;
uint8 pointer = 0;
uint8 bytes[8];
while(flag != true){
while (digitalRead(data_direction) == HIGH) {
  flag = true; 
  if (digitalRead(clk) == LOW){
    previous_clk = LOW;
    }
  if ((digitalRead(clk) == HIGH) && (previous_clk == LOW)){
        bytes[pointer] = digitalRead(data);
        uint8 temp;
        if (pointer == 7){
        pointer = -1;
        temp = 0;
        for (int j = 0; j < 8; j++) {
           uint8 bitshiftedVal = (bytes[j] << (7-j));
           temp |= bitshiftedVal;           
             }              
         receive_buffer.concat(char(temp));
           //or Serial.print(char(temp))
         //Serial.println("receiver hit");   
          } 
       pointer = pointer+1;
       previous_clk = HIGH; 
       //delayMicroseconds(3);   
       }
    }
}
return receive_buffer;    
}

void loop(void) {
  server.handleClient();
}

void transmit(String myText){
pinMode(data, OUTPUT);
pinMode(clk, OUTPUT);
pinMode(data_direction, OUTPUT);
digitalWrite(data_direction, HIGH);
delay(1);
for(int i=0; i<myText.length(); i++){
   char myChar = myText.charAt(i);
 
    for(int i=7; i>=0; i--){
      
      digitalWrite(data, bitRead(myChar, i));
      digitalWrite(clk, HIGH); 
      delayMicroseconds(40);
      digitalWrite(clk, LOW);
      delayMicroseconds(40); 
      //Serial.print(bitRead(myChar, i));
      }
  }
//receive_buffer = "";
digitalWrite(data_direction, LOW);
pinMode(data_direction, INPUT);
pinMode(data, INPUT);
pinMode(data_direction, INPUT);
pinMode(clk, INPUT);
}
