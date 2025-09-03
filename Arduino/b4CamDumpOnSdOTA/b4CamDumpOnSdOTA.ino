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
byte multipleData[4][8];
bool connected=false;
bool finished = false;
int filename = 0;
char filenameArray[12];
File dataDump;

struct lens {
  byte manufacturer[15];
  byte name1[15]; //ASCII Encoding, fill with 0x00 
  byte name2[15];
  byte minFocalLength[2];
  byte maxFocalLength[2];
  byte minAparture[2];
  byte serialNo[5];
  bool extenderAvailable;
  byte MOD[2];
};


lens lensData = {
  {0x44, 0x41, 0x4D, 0x50, 0x46, 0x4B, 0x41, 0x44, 0x53, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x42, 0x49, 0x47, 0x20, 0x44, 0x49, 0x43, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4B},
  {0x2E, 0x2C, 0x2D, 0x28, 0x29, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0xB3, 0x84},
  {0xF1, 0xF4},
  {0xf7, 0xe2},
  {0x36, 0x39, 0x34, 0x32, 0x30},
  true,
  {0x00, 0xFF},
};

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

void lensConnect(){
  if (debug){Serial1.println("Building response for Lens Connect");}
  txData[0] = 0x00;
  txData[1] = 0x01;
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
  sendData();
  return;
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
  txData[txData[0]+2] = calculateChecksum();
  for(byte i=0; i <= txData[0] +2; i++){
    Serial.write(txData[i]);
  }
  for(byte i = 0; i <= rxData[0]+2; i++){
    dataDump.print("0x");
    dataDump.print(rxData[i], HEX);
    dataDump.print("; ");
  }
  dataDump.println();
  clearRxTx();
  return;
}

bool verifyCheckSum() {
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

void setMultipleData(int value) {
  txData[0] = rxData[0];
  txData[1] = rxData[1];
  for(int i=0;i<rxData[0];i++){
    txData[i+2]=rxData[i+2];
    multipleData[value][i]=rxData[i+2];
  }
  sendData();
  return;
}
void getMultipleData(int value) {
  
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

bool receiveData(){
  Serial.setTimeout(2);
  Serial.readBytes(rxData, 18);
  if(verifyCheckSum()) {
    if (debug){Serial1.println("data OK");}
    return true;
  } else {
    if (debug){Serial1.println("data not OK");}
    return false;
  }
}

void getManName(){
  txData[0] = 0x0F;
  txData[1] = 0x10;
  for(byte i = 2; i <= 17; i++) {
    txData[i]=lensData.manufacturer[i-2];
  }
  sendData();
  return;
}

void getLensName(byte x) {
  txData[0] = 0x0F;
  if(x==1){
    txData[1]=0x11;
    for(byte i = 2; i <= 17; i++) {
      txData[i]=lensData.name1[i-2];
    }
  }
  else if(x==2){
    txData[1]=0x12;
    for(byte i = 2; i <= 17; i++) {
      txData[i]=lensData.name2[i-2];
    }
  }
  sendData();
  return;
}

void getOpenFno(){
  if (debug){Serial1.println("Building response for min Aperture");}
  txData[0]=0x02;
  txData[1]=0x13;
  for(byte i = 2; i <= 3; i++) {
    txData[i]=lensData.minAparture[i-2];
  }
  sendData();
  return;
}

void getTeleFocalLength(){
  txData[0]=0x02;
  txData[1]=0x14;
  for(byte i = 2; i <= 3; i++) {
    txData[i]=lensData.minFocalLength[i-2];
  }
  sendData();
  return;
}

void getWideFocalLength() {
  txData[0]=0x02;
  txData[1]=0x15;
  for(byte i = 2; i <= 3; i++) {
    txData[i]=lensData.maxFocalLength[i-2];
  }
  sendData();
  return;
}

void getMod(){
  txData[0]=0x02;
  txData[1]=0x16;
  for(byte i = 2; i <= 3; i++) {
    txData[i]=lensData.MOD[i-2];
  }
  sendData();
  return;
}

void getSerialNo(){
  txData[0]=0x05;
  txData[1]=0x17;
  for(byte i = 2; i <= 6; i++) {
    txData[i]=lensData.serialNo[i-2];
  }
  sendData();
  return;
}

void setIris(){
  txData[0]=0x00;
  txData[1]=0x20;
  sendData();
  return;
}

void setZoom(){
  txData[0]=0x00;
  txData[1]=0x21;
  sendData();
  return;
}

void setFocus(){
  txData[0]=0x00;
  txData[1]=0x22;
  sendData();
  return;
}

void getIris(){
  txData[0]=0x02;
  txData[1]=0x30;
  txData[2]=0x7B;
  txData[3]=0x7B;
  sendData();
  return;
}

void getZoom(){
  txData[0]=0x02;
  txData[1]=0x31;
  txData[2]=0x7B;
  txData[3]=0x7B;
  sendData();
  return;
}

void getFocus(){
  txData[0]=0x02;
  txData[1]=0x32;
  txData[2]=0x7B;
  txData[3]=0x7B;
  sendData();
  return;
}

void loop() {
  ArduinoOTA.handle();
  if(finished){
    digitalWrite(D0, HIGH);
    digitalWrite(D2, HIGH);
    return;
  }
  bool received = false;
  if (Serial.available() > 0) {
    received = receiveData();
  }
  digitalWrite(D0, LOW);
  digitalWrite(D2, HIGH);
  if(received) {
    received = false;
      switch (rxData[1]) {
      case 0x01:
        lensConnect();
        break;
      case 0x10:
        getManName();
        break;
      case 0x11:
        getLensName(0x01);
        break;
      case 0x12:
        getLensName(0x02);
        break;
      case 0x13:
        getOpenFno();
        break;
      case 0x14:
        getTeleFocalLength();
        break;
      case 0x15:
        getWideFocalLength();
        break;
      case 0x16:
        getMod();
        break;
      case 0x17:
        getSerialNo();
        break;
      case 0x20:
        setIris();
        break;
      case 0x21:
        setZoom();
        break;
      case 0x22:
        setFocus();
        break;
      case 0x30:
        getIris();
        break;
      case 0x31:
        getZoom();
        break;
      case 0x32:
        getFocus();
        break;
      case 0x42:
        setSwitch(0x42);
        break;
      case 0x43:
        setSwitch(0x43);
        break;
      case 0x44:
        setSwitch(0x44);
        break;
      case 0x52:
        getSwitch(0x52);
        break;
      case 0x53:
        getSwitch(0x53);
        break;
      case 0x54:
        getSwitch(0x54);
        break;
      case 0x60:
        getMultipleData(0);
        break;
      case 0x61:
        getMultipleData(1);
        break;
      case 0x62:
        getMultipleData(2);
        break;
      case 0x63:
        getMultipleData(3);
        break;
      case 0x70:
        setMultipleData(0);
        break;
      case 0x71:
        setMultipleData(1);
        break;
      case 0x72:
        setMultipleData(2);
        break;
      case 0x73:
        setMultipleData(3);
        break;
      default:
        txData[0]=0x00;
        txData[1]=rxData[1];
        sendData();
        break;
    }
    if(millis()/100%2){
      digitalWrite(D2, LOW);
      digitalWrite(D0, HIGH);
    } else {digitalWrite(D2, HIGH);digitalWrite(D0, LOW);}
  if(millis()>120000){
    dataDump.close();
    finished = true;
    connected = false;
  }
}
