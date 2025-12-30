// Basic demo for configuring the MCP4728 4-Channel 12-bit I2C DAC
#include <Adafruit_MCP4728.h>
#include <Adafruit_ADS1X15.h>
#include <Wire.h>
#include <PCF8575.h>  
uint32_t iris_set = random(140, 3200); //(/100 to get actual F Stop)
uint32_t focus_set = random(60, 1500000); //Focal Distance in cm
int zoom_set = random(9, 500); //(Focal length in mm)
float iris_get = random(140, 3200) / 100.00;
int focus_get = random(0, 2048);
int zoom_get = random(9, 500);
bool ext = random(0, 1);
bool tally = random(0, 1);
bool iso = 0;
bool call = 0;
bool preview = 0;
bool ret = random(0, 1);
bool vtr = random(0, 1);
bool aux = random(0, 1);
bool prod = 0;
bool eng = 0;
bool tbd = 0;
Adafruit_MCP4728 mcp;
Adafruit_ADS1015 ads; 
PCF8575 PCF(0x20); 

void setup(void) {
  Serial.begin(115200);
  Wire.begin(8,9,400000);
  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS1115");
    while (1);
  }
  if (!mcp.begin()) {
    Serial.println("Failed to initialize MCP4728");
    while (1) {
      delay(10);
    }
  }
  mcp.begin();
  ads.begin();
  PCF.begin();

  //set initial values
  mcp.setChannelValue(MCP4728_CHANNEL_A, map(iris_set,140,3200,0,4096));
  mcp.setChannelValue(MCP4728_CHANNEL_B, map(zoom_set,9,500,0,4096));
  mcp.setChannelValue(MCP4728_CHANNEL_C, map(focus_set,60,1500000,0,4096));
  mcp.setChannelValue(MCP4728_CHANNEL_D, 0);
}

void readAnalog(){
  iris_get = map(ads.readADC_SingleEnded(0), 0, 65535, 140, 3200);
  zoom_get = map(ads.readADC_SingleEnded(1), 0, 65535, 9, 500);
  focus_get = map(ads.readADC_SingleEnded(2), 0, 65535, 60, 1500000); //JUST A GUESS, NEEDS TO BE MAPPED
  //adc3 = ads.readADC_SingleEnded(3);
}

void writeAnalog() {
  mcp.setChannelValue(MCP4728_CHANNEL_A, map(iris_set,140,3200,0,4096));
  mcp.setChannelValue(MCP4728_CHANNEL_B, map(zoom_set,9,500,0,4096));
  mcp.setChannelValue(MCP4728_CHANNEL_C, map(focus_set,60,1500000,0,4096));
}

void readSwitches() {
ret =  PCF.read(13);
prod =  PCF.read(15);
eng =  PCF.read(14);
tbd =  PCF.read(12);
}

void writeLEDs() {
  PCF.write(0,!call);
  PCF.write(1,!iso);
  PCF.write(2,!preview);
  PCF.write(3,!tally);
}

void ledDemo() {
  if((millis()/250)%4==0){
    tally = 1;
    iso = 0;
    preview = 0;
    call = 0;
  }
  if((millis()/250)%4==1){
    tally = 0;
    iso = 1;
    preview = 0;
    call = 0;
  }
  if((millis()/250)%4==2){
    tally = 0;
    iso = 0;
    preview = 1;
    call = 0;
  }
  if((millis()/250)%4==3){
    tally = 0;
    iso = 0;
    preview = 0;
    call = 1;
  }
  writeLEDs();
}

void loop() {
  readSwitches();
  if(!tbd){
    ledDemo();
  } else {tally=0; iso=0; preview=0; call = 0; writeLEDs();}
}
