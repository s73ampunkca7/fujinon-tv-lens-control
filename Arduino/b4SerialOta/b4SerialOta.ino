bool debug = false;

struct lens {
  byte manufacturer[15];
  byte name1[15]; //ASCII Encoding, fill with 0x00 
  byte name2[15];
  byte minFocalLength[2];
  byte maxFocalLength[2];
  byte minAparture[2];
  bool extenderAvailable;
  byte MOD[2];
  byte serial[15];
  int32_t irisTarget;
  int32_t irisFeedback;
  int32_t zoomTarget;
  int32_t zoomFeedback;
  int32_t focusTarget;
  int32_t focusFeedback;
  bool extFeedback;
  bool tally;
  bool iso;
  bool call;
  bool preview;
  bool retFeedback;
  bool vtrFeedback;
  bool auxFeedback;
  bool prodFeedback;
  bool engFeedback;
  bool tbdFeedback;
};


lens lensData = {
  {0x43, 0x41, 0x4E, 0x4F, 0x4E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //manufacuturer
  {0x4A, 0x35, 0x35, 0x78, 0x39, 0x20, 0x53, 0x55, 0x50, 0x45, 0x52, 0x00, 0x00, 0x00, 0x00}, //Lens Name 1
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //lens Name 2
  {0xB3, 0x84}, //min Focal Length
  {0xD1, 0xF4}, //max Focal Length
  {0xf0, 0x78}, //min F stop
  true, //extender?
  {0xD2, 0x58}, //Minimum Object Distance
  {0x32, 0x31, 0x39, 0x30, 0x32, 0x41, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //S/N
};

//available lenses:

byte rxData[18];
byte txData[18];
byte rxDataLength;
byte txDataLength;

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

void lensConnect(){
  if (debug){Serial1.println("Building response for Lens Connect");}
  txData[0] = 0x00;
  txData[1] = 0x01;
  sendData();
  return;
}

void getManName(){
  if (debug){Serial1.println("Building response for Manufacturer Name");}
  txData[0] = 0x0F;
  txData[1]=0x10;
  for(byte i = 2; i <= 17; i++) {
    txData[i]=lensData.manufacturer[i-2];
  }
  sendData();
  return;
}

void getLensName(byte x) {
  if (debug){Serial1.println("Building response for Lens Name");}
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
  if (debug){Serial1.println("Building response for Tele focal length");}
  txData[0]=0x02;
  txData[1]=0x14;
  for(byte i = 2; i <= 3; i++) {
    txData[i]=lensData.maxFocalLength[i-2];
  }
  sendData();
  return;
}

void getWideFocalLength(){
  if (debug){Serial1.println("Building response for wide focal length");}
  txData[0]=0x02;
  txData[1]=0x15;
  for(byte i = 2; i <= 3; i++) {
    txData[i]=lensData.minFocalLength[i-2];
  }
  sendData();
  return;
}


void getMod(){
  if (debug){Serial1.println("Building response for MOD");}
  txData[0]=0x02;
  txData[1]=0x16;
  for(byte i = 2; i <= 3; i++) {
    txData[i]=lensData.MOD[i-2];
  }
  sendData();
  return;
}

void getSerial(){
  if (debug){Serial1.println("Building response for Serialnumber");}
  txData[0] = 0x0F;
  txData[1]=0x17;
  for(byte i = 2; i <= 17; i++) {
    txData[i]=lensData.serial[i-2];
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

}

void setFocus(){

}

void getIris(){

}

void getZoom(){

}

void getFocus(){

}

void setSwitch(byte x){
  txData[0]=0x00;
  txData[1]=x;
  sendData();
  return;
}

void getSwitch(byte x){
  txData[0]=0x01;
  txData[1]=x;
  txData[2]=0x04;
  sendData();
  return;
}

void getMultipleData() {
  txData[0] = 0x0C;
  txData[1] = rxData[0];
  txData[2] = 0x00; //does nothing?
  txData[3] = 0x00; //does nothing?
  txData[4] = 0xFF; //iris
  txData[5] = 0xFF; //iris
  txData[6] = 0x00; //nothing
  txData[7] = 0x00; //nothing
  txData[8] = 0x42;
  txData[9] = 0x42;
  txData[10] = 0x42;
  txData[11] = 0x42;
  txData[12] = 0x42;
  txData[13] = 0x42;
  sendData();
  return;
}

void setMultipleData() {
  //TODO: Safe these values for 0x60
  txData[0] = rxDataLength;
  txData[1] = rxData[0];
  for(int i=0;i<rxDataLength;i++){
    txData[i+2]=rxData[i+1];
  }
  sendData();
  return;
}
void someSortOfAck() {
  txData[0] = 0x00;
  txData[1] = 0x05;
  sendData();
  return;
}

void loop() {
  ArduinoOTA.handle();
  bool received = false;
    if (Serial.available() > 0) {
      received = receiveData();
  }
  if(received) {
    received = false;
  if (debug==true){Serial1.println("Checksum verified, ready to answer");}
    switch (rxData[0]) {
      case 0x01:
        lensConnect();
        break;
      case 0x05:
        someSortOfAck();
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
        getSerial();
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
      case 0x23:
        setIrisAbs();
        break;
      case 0x24:
        setZoomAbs();
        break;
      case 0x25:
        setFocusAbs();
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
        getMultipleData();
        break;
      case 0x61:
        getMultipleData();
        break;
      case 0x62:
        getMultipleData();
        break;
      case 0x70:
        setMultipleData();
        break;
      case 0x71:
        setMultipleData();
        break;
      case 0x72:
        setMultipleData();
        break;
    }
  }
}
