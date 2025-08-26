#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#ifndef STASSID
#define STASSID "TP-Link_5B09"
#define STAPSK "95271750"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
bool debug = false;
bool connected = false;

struct lens {
  byte manufacturer[15];
  byte name1[15]; //ASCII Encoding, fill with 0x00 
  byte name2[15];
  byte minFocalLength[2];
  byte maxFocalLength[2];
  byte minAparture[2];
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
  true,
  {0x00, 0xFF},
};

//available lenses:

byte rxData[18];
byte txData[18];
byte response[18];
byte rxDataLength;
byte txDataLength;
byte commandByte =0x00;
bool finished = false;


void setup() {
  if(debug){Serial1.begin(115200);}
  Serial.begin(78400);
  if (debug){Serial1.println("Hello world!");}
  if(debug){Serial1.println("Booting");}
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    if(debug){Serial1.println("Connection Failed! Rebooting...");}
    delay(5000);
    ESP.restart();
  }


  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    if(debug){Serial1.println("Start updating " + type);}
  });
  ArduinoOTA.onEnd([]() {
    if(debug){Serial1.println("\nEnd");}
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    if(debug){Serial1.printf("Progress: %u%%\r", (progress / (total / 100)));}
  });
  ArduinoOTA.onError([](ota_error_t error) {
    if(debug){Serial1.printf("Error[%u]: ", error);}
    if (error == OTA_AUTH_ERROR) {
      if(debug){Serial1.println("Auth Failed");}
    } else if (error == OTA_BEGIN_ERROR) {
      if(debug){Serial1.println("Begin Failed");}
    } else if (error == OTA_CONNECT_ERROR) {
      if(debug){Serial1.println("Connect Failed");}
    } else if (error == OTA_RECEIVE_ERROR) {
      if(debug){Serial1.println("Receive Failed");}
    } else if (error == OTA_END_ERROR) {
      if(debug){Serial1.println("End Failed");}
    }
  });
  ArduinoOTA.begin();
  if(debug){Serial1.println("Ready");}
  if(debug){Serial1.print("IP address: ");}
  if(debug){Serial1.println(WiFi.localIP());}
}
bool receiveData() {
  if (debug){Serial1.println("receiving Data");}
  rxDataLength = Serial.read();
  Serial.readBytes(rxData, (rxDataLength + 2));
  if (debug){
    Serial1.print("Data: 0x");
    Serial1.print(rxDataLength, HEX);
    for(byte i=0; i<rxDataLength+2;i++){
      Serial1.print(" 0x");
      Serial1.print(rxData[i], HEX);
    }
    Serial1.println("");
  }
  if(verifyCheckSum()) {
    if (debug){Serial1.println("data OK");}
    return true;
  } else {
    if (debug){Serial1.println("data not OK");}
    return false;
  }
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
  rxDataLength=0x00;
  txDataLength=0x00;
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
  byte checksum = rxDataLength;
  for(byte i=0; i <= rxDataLength + 2; i++) {
    checksum = checksum + rxData[i];
  }
  if(checksum == 0) {
    return true;
  }
  else {
    return false;
    }
}

void checkCommand(){
    txData[0]=0x00;
    txData[1]= commandByte;
    sendData();
    getResponse();
    return;
}
void getResponse(){
    Serial.setTimeout(10);
    Serial.readBytes(response, 18);
}

bool sendConnect(){
    txData[0]=0x00;
    txData[1]=0x01;
    sendData();
    getResponse();
    if(response[1]==0x01){
        return true;
    } else {return false;}
}

void loop() {
  ArduinoOTA.handle();
  while(!connected){
    connected = sendConnect();
    ArduinoOTA.handle();
  }
    if(!finished){
        for(byte i = 0x02; i <= 0xFF; i++){
          commandByte++;
            checkCommand();
            for(byte k=0; k<=17;k++){
              Serial1.print(response[k]);
            }
            Serial1.println("");
            if(i=0xFF){finished=true;}
        }
    }
}
