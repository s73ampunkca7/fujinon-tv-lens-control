#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <SD.h>
#include <SPI.h>


// Replace with your network credentials
#ifndef STASSID
#define STASSID "TP-Link_5B09"
#define STAPSK "95271750"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
bool debug = false;
byte rxData[18];
byte txData[18];
bool connected=false;
bool finished = false;
int filename = 0;
char filenameArray[12];
File dataDump;


void setup() {
  Serial1.begin(115200);
  Serial.begin(76800);
  Serial1.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial1.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  if (!SD.begin(15)) {
    Serial1.println("initialization failed!");
    while (1);
  }
  
  ArduinoOTA.onStart([]() {
    Serial1.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial1.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial1.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial1.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial1.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial1.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial1.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial1.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial1.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial1.println("Ready");
  Serial1.print("IP address: ");
  Serial1.println(WiFi.localIP());
  
  pinMode(D0, OUTPUT);
  pinMode(D2, OUTPUT);
  for(int i=0; i<=7; i++){
    filenameArray[i]=(char)random(65, 90);
  }
  digitalWrite(D0, HIGH); //Blinking== ERROR; ON == waiting for connection
  digitalWrite(D2, LOW); //Blinking == Dumping; ON == Finished
  delay(250);
  digitalWrite(D0, LOW); //Blinking== ERROR; ON == waiting for connection
  digitalWrite(D2, HIGH); //Blinking == Dumping; ON == Finished
  delay(250);
  digitalWrite(D0, HIGH); //Blinking== ERROR; ON == waiting for connection
  digitalWrite(D2, LOW); //Blinking == Dumping; ON == Finished
  delay(250);
  digitalWrite(D0, LOW); //Blinking== ERROR; ON == waiting for connection
  digitalWrite(D2, HIGH); //Blinking == Dumping; ON == Finished
  delay(250);
  digitalWrite(D0, HIGH); //Blinking== ERROR; ON == waiting for connection
  digitalWrite(D2, LOW); //Blinking == Dumping; ON == Finished
  delay(250);
  digitalWrite(D0, LOW); //Blinking== ERROR; ON == waiting for connection
  digitalWrite(D2, HIGH); //Blinking == Dumping; ON == Finished
  delay(250);
  digitalWrite(D0, HIGH); //Blinking== ERROR; ON == waiting for connection
  digitalWrite(D2, LOW); //Blinking == Dumping; ON == Finished
  delay(250);
  digitalWrite(D0, LOW); //Blinking== ERROR; ON == waiting for connection
  digitalWrite(D2, HIGH); //Blinking == Dumping; ON == Finished
  delay(250);
  digitalWrite(D0, HIGH); //Blinking== ERROR; ON == waiting for connection
  digitalWrite(D2, LOW); //Blinking == Dumping; ON == Finished
  delay(250);
  
}

byte calculateChecksum() {
  if (debug){Serial1.println("calculating tx Checksum");}
  byte checksum = 0x00;
  for(byte i=0; i <= txData[0] + 1; i++) {
    checksum = checksum + txData[i];
    if (debug){
      Serial1.print("sum of Data: 0x");
      Serial1.println(checksum, HEX);
    }
  }
  checksum = 0x100 - checksum;
  if (debug){
    Serial1.print("Final Checksum: 0x");
    Serial1.println(checksum, HEX);
  }
  return checksum;
}

void clearRxTx() {
  if (debug){Serial1.println("clearing rx and tx data..");}
  for(byte i=0; i<18; i++){
    rxData[i]=0x00;
    txData[i]=0x00;
  }
  return;
}

void sendData(){
  if(debug){
    Serial1.print("tx Data:");
    for(byte i=0; i<=txData[0]+2;i++){
      Serial.print(" 0x");
      Serial1.print(txData[i], HEX);
    }
    Serial1.println("");
    Serial1.println("sending raw Data...");
  }
  txData[txData[0]+2] = calculateChecksum();
  for(byte i=0; i <= txData[0] +2; i++){
    Serial.write(txData[i]);
  }
  clearRxTx();
  return;
}

bool verifyCheckSum() {
  if (debug){Serial1.println("verifying Checksum...");}
  byte checksum;
  for(byte i=0; i <= rxData[0] + 2; i++) {
    checksum = checksum + rxData[i];
  }
  if(checksum == 0) {
    return true;
  }
  else {
    return false;
    }
}

void getResponse(){
    Serial.setTimeout(15);
    Serial.readBytes(rxData, 18);
    return;
}



bool connect(){
    txData[0]=0x00;
    txData[1]=0x01;
    sendData();
    getResponse();
    if(rxData[1]==0x01){
        return true;
    } else {return false;}
}

void loop() {
  ArduinoOTA.handle();
  if(finished){
    digitalWrite(D0, HIGH);
    digitalWrite(D2, HIGH);
    return;
  }
  while(!connected){
    connected = connect();
    return;
  }
  digitalWrite(D0, LOW);
  digitalWrite(D2, HIGH);
  while(SD.exists(filenameArray)){
    for(int i=0; i<=7; i++){
      filenameArray[i]=(char)random(65, 90);
    }
  }
  filenameArray[0] = (char) 47;
  filenameArray[8] = (char) 46;
  filenameArray[9] = (char) 84;
  filenameArray[10] = (char) 88;
  filenameArray[11] = (char) 84;
  dataDump = SD.open(filenameArray, FILE_WRITE);
  for(int i = 0; i <=0xFF; i++){
    if(millis()/100%2){
        digitalWrite(D2, LOW);
        digitalWrite(D0, HIGH);
    } else {digitalWrite(D2, HIGH);digitalWrite(D0, LOW);}
    txData[1] = i;
    sendData();
    getResponse();
    if (true){
      Serial1.print("Writing to file...");
      for(byte i = 0; i <= rxData[0]+2; i++){
        dataDump.print("0x");
        dataDump.print(rxData[i], HEX);
        dataDump.print("; ");
      }
      if(rxData[1]==0x11){
        for(byte j = 2; j <= rxData[0]+2; j++){
          dataDump.print((char)rxData[j]);
        }
      }
    dataDump.println();
    }
  }
  dataDump.close();
  finished = true;
  connected = false;
}
