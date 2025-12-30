#include <TFT_eSPI.h>
#include "images.h"
#include <Adafruit_MCP4728.h>
#include <Adafruit_ADS1X15.h>
#include <Wire.h>
#include <PCF8575.h> 
#define CANON_HEIGHT 79
#define CANON_WIDTH 320

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240


String manufacturer = "CANON";
String lensname = "J55x9 SUPER";

int32_t iris_set = 140;
int32_t focus_set = 60;
int zoom_set = 9;
int32_t iris_get = 140;
int32_t iris_get_raw =0;
float focus_get = 60;
int32_t focus_get_raw = 0;
int zoom_get = 9;
int32_t zoom_get_raw = 0;
int32_t aux_raw = 0;
bool ext = 0;
bool tally = 0;
bool iso = 0;
bool call = 0;
bool preview = 0;
bool ret = 0;
bool vtr = 0;
bool aux = 0;
bool prod = 0;
bool eng = 0;
bool tbd = 0;



int menu = 0;           //0=MAIN; 1=OVERVIEW; 2=VISUAL
int submenu = 0;
uint16_t x = 0, y = 0;  // To store the touch coordinates
bool demo_zoom_dir = 0;
bool demo_iris_dir = 0;
int prev_comp = 0;
int lastUpdate;
int updateRate = 50; //period (ms) between drawing frames 
int rainbowIndex;

//PAGE 1 var Positions
#define IRIS_SET_X_POS 120
#define IRIS_SET_Y_POS 115
#define IRIS_GET_X_POS 280
#define IRIS_GET_Y_POS 115

#define FOCUS_SET_X_POS 120
#define FOCUS_SET_Y_POS 132
#define FOCUS_GET_X_POS 280
#define FOCUS_GET_Y_POS 132

#define ZOOM_SET_X_POS 120
#define ZOOM_SET_Y_POS 149
#define ZOOM_GET_X_POS 280
#define ZOOM_GET_Y_POS 149
#define BIT_1_X_POS 80
#define BIT_1_Y_POS 166
#define BIT_2_X_POS 187
#define BIT_2_Y_POS 166
#define BIT_3_X_POS 293
#define BIT_3_Y_POS 166
#define BIT_4_X_POS 80
#define BIT_4_Y_POS 184
#define BIT_5_X_POS 187
#define BIT_5_Y_POS 184
#define BIT_6_X_POS
#define BIT_6_Y_POS
#define BIT_7_X_POS
#define BIT_7_Y_POS
#define BIT_8_X_POS
#define BIT_8_Y_POS
#define BIT_9_X_POS
#define BIT_9_Y_POS
#define BIT_10_X_POS
#define BIT_10_Y_POS
#define BIT_11_X_POS
#define BIT_11_Y_POS
#define BIT_12_X_POS
#define BIT_12_Y_POS
#define BIT_13_X_POS
#define BIT_13_Y_POS
#define BIT_14_X_POS
#define BIT_14_Y_POS
#define BIT_15_X_POS
#define BIT_15_Y_POS
#define BIT_16_X_POS
#define BIT_16_Y_POS



Adafruit_MCP4728 mcp;
Adafruit_ADS1115 ads; 
PCF8575 PCF(0x20); 
TFT_eSPI tft = TFT_eSPI();




void drawCWTV() {
  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
  tft.fillRect(267, 218, 53, 22, TFT_MAGENTA);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("MENU", 293, 222, 2);
  for (int i = 0; i <= 130; i++) {
    for (int j = 0; j <= 320; j++) {
      if (cwtv[i][j + 1] == 0) {
        tft.drawPixel(j, i+55, TFT_WHITE);
      }
    }
  }
}

void drawLogo() {
  for (int i = 0; i <= 77; i++) {
    for (int j = 0; j <= 320; j++) {
      if (canon[i][j + 1] == 0) {
        tft.drawPixel(j, i, TFT_RED);
      }
    }
  }
}

void drawLenstype(int height) {
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawLine(0, height, 320, height, TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(lensname, 315, height + 4);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(manufacturer, 4, height + 4);
  tft.drawLine(0, height + 21, 320, height + 21, TFT_WHITE);
  tft.drawLine(0, height + 22, 320, height + 22, TFT_WHITE);
}

void drawSubMenus() {
  tft.setTextSize(1);
  tft.setTextDatum(TC_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.fillRect(267, 218, 53, 22, TFT_MAGENTA);
  tft.fillRect(214, 218, 52, 22, TFT_BLUE);
  tft.fillRect(161, 218, 52, 22, TFT_GREEN);
  tft.fillRect(107, 218, 53, 22, TFT_YELLOW);
  tft.fillRect(54, 218, 52, 22, TFT_ORANGE);
  tft.fillRect(0, 218, 53, 22, TFT_RED);
  tft.drawString("ADC", 27, 222, 2);
  tft.drawString("SERIAL", 80, 222, 2);
  tft.drawString("SUB3", 133, 222, 2);
  tft.drawString("SUB4", 186, 222, 2);
  tft.drawString("SUB5", 239, 222, 2);
  tft.drawString("MENU", 293, 222, 2);
}

void changeMenu() {
  if (menu == 0) {
    if (x >= 4 && x <= 106 && y >= 103 && y <= 153) {
      tft.fillSmoothRoundRect(4, 103, 100, 50, 5, TFT_WHITE);
      delay(100);
      drawOverview();
      menu = 1;
    } else if (x >= 110 && x <= 212 && y >= 103 && y <= 153) {
      tft.fillSmoothRoundRect(110, 103, 100, 50, 5, TFT_WHITE);
      delay(100);
      drawVisual();
      menu = 2;
    } else if (x >= 216 && x <= 336 && y >= 103 && y <= 153) {
      tft.fillSmoothRoundRect(216, 103, 100, 50, 5, TFT_WHITE);
      delay(100);
      drawCWTV();
      menu = 3;
    }
  }
  if (menu == 1) {
    if (x >= 267 && x <= 320 && y >= 218 && y <= 240) { //Mainmenu
      tft.fillRect(267, 218, 53, 22, TFT_WHITE);
      delay(100);
      drawMainMenu();
      submenu = 0;
      menu = 0;
    }
    if (x >= 0 && x <= 53 && y >= 218 && y <= 240) { //ADC Readings
      tft.fillRect(0, 218, 53, 22, TFT_WHITE);
      delay(100);
      submenu = 1;
      drawOverviewSub();
    }
    if (x >= 54 && x <= 106 && y >= 218 && y <= 240) { //Serial Readings
      tft.fillRect(54, 218, 52, 22, TFT_WHITE);
      delay(100);
      submenu = 2;
      drawOverviewSub();
    }
    if (x >= 107 && x <= 160 && y >= 218 && y <= 240) { //Submenu 3
      tft.fillRect(107, 218, 53, 22, TFT_WHITE);
      delay(100);
      submenu = 3;
      drawOverviewSub();
    }
    if (x >= 161 && x <= 213 && y >= 218 && y <= 240) { //Submenu 4
      tft.fillRect(161, 218, 52, 22, TFT_WHITE);
      delay(100);
      submenu = 4;
      drawOverviewSub();
    }
    if (x >= 214 && x <= 266 && y >= 218 && y <= 240) { //Submenu 5
      tft.fillRect(214, 218, 52, 22, TFT_WHITE);
      delay(100);
      submenu = 5;
      drawOverviewSub();
    }
  }
  if (menu == 2) {
    if (x >= 216 && x <= 336 && y >= 30 && y <= 80) {
      tft.fillSmoothRoundRect(216, 30, 100, 50, 5, TFT_WHITE);
      delay(100);
      drawMainMenu();
      menu = 0;
    }
  }
  if (menu == 3) {
    if (x >= 267 && x <= 320 && y >= 218 && y <= 240) {
      tft.fillRect(267, 218, 53, 22, TFT_WHITE);
      delay(100);
      drawMainMenu();
      menu = 0;
    }
  }
  return;
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
  if(rainbowIndex>47){rainbowIndex=0;}
  rgbLedWrite(21, RGBarray[rainbowIndex][0],RGBarray[rainbowIndex][1],RGBarray[rainbowIndex][2]);
  rainbowIndex=rainbowIndex+1;
}

void drawOverview() {
  tft.fillRect(0, 97, 320, 143, TFT_BLACK);
  drawLogo();
  drawLenstype(75);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawLine(160, 114, 160, 164, TFT_WHITE);
  tft.drawLine(159, 114, 159, 164, TFT_WHITE);
  tft.drawLine(80, 114, 80, 164, TFT_WHITE);
  tft.drawLine(240, 114, 240, 164, TFT_WHITE);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("LIVE OVERVIEW", 160, 98, 2);
  tft.drawLine(0, 114, 320, 114, TFT_WHITE);
  tft.drawString("IRIS_SET", 40, 115, 2);
  tft.drawString("IRIS_GET", 200, 115, 2);
  tft.drawLine(0, 131, 320, 131, TFT_WHITE);
  tft.drawString("FOCUS_SET", 40, 132, 2);
  tft.drawString("FOCUS_GET", 200, 132, 2);
  tft.drawLine(0, 148, 320, 148, TFT_WHITE);
  tft.drawString("ZOOM_SET", 40, 149, 2);
  tft.drawString("ZOOM_GET", 200, 149, 2);
  tft.drawLine(0, 165, 320, 165, TFT_WHITE);
  tft.drawLine(53, 166, 53, 240, TFT_WHITE);
  tft.drawLine(106, 166, 106, 240, TFT_WHITE);
  tft.drawLine(105, 166, 105, 240, TFT_WHITE);
  tft.drawLine(160, 166, 160, 240, TFT_WHITE);
  tft.drawLine(213, 166, 213, 240, TFT_WHITE);
  tft.drawLine(212, 166, 212, 240, TFT_WHITE);
  tft.drawLine(266, 166, 266, 240, TFT_WHITE);
  tft.drawLine(0, 183, 320, 183, TFT_WHITE);
  tft.drawLine(0, 200, 320, 200, TFT_WHITE);
  tft.drawLine(0, 217, 320, 217, TFT_WHITE);
  tft.drawString("PROD", 27, 166, 2);
  tft.drawString("ENG", 133, 166, 2);
  tft.drawString("RET", 240, 166, 2);
  tft.drawString("TBD", 27, 184, 2);
  tft.drawString("XXX", 133, 184, 2);
  drawSubMenus();
}

void drawOverviewSub(){
  tft.setTextSize(2);
  tft.setTextDatum(CC_DATUM);
  tft.fillRect(0, 97, 320, 120, TFT_BLACK);
  drawSubMenus();
  switch(submenu) {
    case(1):
    tft.drawString("ADC READINGS", 160, 98, 2);
    tft.drawString("IRIS", 80, 132, 2);
    tft.drawString("ZOOM", 80, 149, 2);
    tft.drawString("FOCUS", 80, 166, 2);
    tft.drawString("RAW", 160, 115, 2);
    tft.drawString("ADJ.", 240, 115, 2);
    tft.drawString("AUX", 80, 200, 2);
    tft.drawLine(40, 114, 280, 114, TFT_WHITE);
    tft.drawLine(40, 131, 280, 131, TFT_WHITE);
    tft.drawLine(40, 148, 280, 148, TFT_WHITE);
    tft.drawLine(40, 165, 280, 165, TFT_WHITE);
    tft.drawLine(40, 182, 280, 182, TFT_WHITE);
    tft.drawLine(40, 114, 40, 182, TFT_WHITE);
    tft.drawLine(280, 114, 280, 182, TFT_WHITE);
    tft.drawLine(120, 114, 120, 182, TFT_WHITE);
    tft.drawLine(200, 114, 200, 182, TFT_WHITE);
    break;
    case(2):
    tft.drawString("SERIAL COMM. DATA", 160, 120, 2);
    break;
    case(3):
    tft.drawString("SUBMENU 3", 160, 120, 2);
    break;
    case(4):
    tft.drawString("SUBMENU 4", 160, 120, 2);
    break;
    case(5):
    tft.drawString("SUBMENU 5", 160, 120, 2);
    break;
  }
}

void drawMainMenu() {
  tft.fillScreen(TFT_BLACK);  // Fill the screen with black color
  drawLogo();
  drawLenstype(75);
  tft.fillSmoothRoundRect(4, 103, 100, 50, 5, TFT_RED);
  tft.fillSmoothRoundRect(110, 103, 100, 50, 5, TFT_GREEN);
  tft.fillSmoothRoundRect(216, 103, 100, 50, 5, TFT_BLUE);
  tft.fillSmoothRoundRect(4, 157, 100, 50, 5, TFT_CYAN);
  tft.fillSmoothRoundRect(110, 157, 100, 50, 5, TFT_MAGENTA);
  tft.fillSmoothRoundRect(216, 157, 100, 50, 5, TFT_YELLOW);
  tft.setTextDatum(CC_DATUM);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_RED);
  tft.drawString("OVERVIEW", 54, 128);
  tft.setTextColor(TFT_BLACK, TFT_GREEN);
  tft.drawString("VISUAL", 160, 128);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.drawString("CWTV", 267, 128);
  tft.setTextColor(TFT_BLACK, TFT_CYAN);
  tft.drawString("MENU 4", 54, 182);
  tft.setTextColor(TFT_WHITE, TFT_MAGENTA);
  tft.drawString("MENU 5", 160, 182);
  tft.setTextColor(TFT_BLACK, TFT_YELLOW);
  tft.drawString("MENU 6", 267, 182);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  menu = 0;
}

void drawVisual() {
  tft.fillScreen(TFT_BLACK);  // Fill the screen with black color
  drawLenstype(0);
  tft.fillSmoothRoundRect(216, 30, 100, 50, 5, TFT_BLUE);
  tft.setTextDatum(CC_DATUM);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.drawString("MENU", 267, 45);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextDatum(CC_DATUM);
  tft.drawString("Focus:", 45, 75);
  tft.drawString("Zoom:", 160, 115);
  tft.drawString("Iris:", 233, 115);
  for (int i = 0; i <= 143; i++) {
    for (int j = 0; j <= 320; j++) {
      if (lenses_static[i][j] == 0) {
        tft.drawPixel(j, i + 95, TFT_WHITE);
      }
    }
  }
  tft.drawLine(0, 163, 320, 163, TFT_RED);
  tft.drawLine(233, 145, 233, 158, TFT_RED);
  tft.drawLine(233, 169, 233, 183, TFT_RED);
}

int scaleZoom() {
  int comp = 0;
  comp = map(zoom_get, 9, 500, 0, -51);
  return comp;
}

void updateVisual() {
  int comp = scaleZoom();
  tft.setTextSize(1);
  tft.setTextDatum(CC_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("     ", 45, 85);
  tft.drawString(String(focus_get), 45, 85);
  tft.drawString("     ", 160, 125);
  tft.drawString(String(zoom_get) + "mm", 160, 125);
  tft.drawString("        ", 233, 125);
  tft.drawString("f/" + String(iris_get / 100.00), 233, 125);
  if (comp != prev_comp) {
    for (int i = 0; i <= 36; i++) {
      for (int j = 0; j <= 22; j++) {
        if (variator_minimal[i][j] == 0) {
          tft.drawPixel(j + 70 - comp, i + 145, TFT_WHITE);
        } else {
          tft.drawPixel(j + 70 - comp, i + 145, TFT_BLACK);
        }
      }
    }
    for (int i = 0; i <= 55; i++) {
      for (int j = 0; j <= 35; j++) {
        if (compensator_minimal[i][j] == 0) {
          tft.drawPixel(j + 197 + comp, i + 136, TFT_WHITE);
        } else {
          tft.drawPixel(j + 197 + comp, i + 136, TFT_BLACK);
        }
      }
    }
    tft.drawLine(0, 163, 320, 163, TFT_RED);
    prev_comp = comp;
  }
}

void setup() {
  uint16_t calData[5] = { 418, 3480, 269, 3509, 5 };
  Serial.begin(115200);
  delay(1000);
  Wire.begin(8,9,400000);
  mcp.begin();
  ads.begin();
  PCF.begin();
  ads.setGain(GAIN_ONE);
  tft.setTouch(calData);
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);  // Fill the screen with black color
  Serial.println("Start");
  tft.setTextColor(TFT_WHITE, TFT_TRANSPARENT);
  drawMainMenu();
  drawLenstype(75);
}

void generateDummyValues() {
  iris_set = random(140, 3200) / 100.00;
  focus_set = random(0, 2048);
  zoom_set = random(9, 500);
  focus_get = random(0, 2048);
  if (demo_zoom_dir == 0) {
    zoom_get = zoom_get + 2;
    if (zoom_get >= 500) {
      demo_zoom_dir = 1;
    }
  }
  if (demo_zoom_dir == 1) {
    zoom_get = zoom_get - 2;
    if (zoom_get <= 9) {
      demo_zoom_dir = 0;
    }
  }

  if (demo_iris_dir == 0) {
    iris_get = iris_get + 10;
    if (iris_get >= 3200) {
      demo_iris_dir = 1;
    }
  }
  if (demo_iris_dir == 1) {
    iris_get = iris_get - 10;
    if (iris_get <= 140) {
      demo_iris_dir = 0;
    }
  }
  ext = random(0, 2);
  tally = random(0, 2);
  ret = random(0, 2);
  vtr = random(0, 2);
  aux = random(0, 2);
}

void readAnalog(){
  iris_get_raw = ads.readADC_SingleEnded(0);
  zoom_get_raw = ads.readADC_SingleEnded(1);
  focus_get_raw = ads.readADC_SingleEnded(2);
  aux_raw = ads.readADC_SingleEnded(3);
  iris_get = map(iris_get_raw, 0, 65535, 140, 3200);
  zoom_get = map(zoom_get_raw, 0, 26210, 9, 500);
  focus_get = map(focus_get_raw, 0, 26210, 0.6, 15000); //JUST A GUESS, NEEDS TO BE MAPPED
  //adc3 = ads.readADC_SingleEnded(3);
}

void writeAnalog() {
  mcp.setChannelValue(MCP4728_CHANNEL_A, map(iris_set,140,3200,0,4096));
  mcp.setChannelValue(MCP4728_CHANNEL_B, map(zoom_set,9,500,0,4096));
  mcp.setChannelValue(MCP4728_CHANNEL_C, map(focus_set,60,1500000,0,4096));
}

void updateSub1() {
    tft.drawString("       ", 160, 132, 2);
    tft.drawString(String(iris_get_raw), 160, 132, 2);
    tft.drawString("       ", 160, 149, 2);
    tft.drawString(String(zoom_get_raw), 160, 149, 2);
    tft.drawString("       ", 160, 166, 2);
    tft.drawString(String(focus_get_raw), 160, 166, 2);
    tft.drawString("       ", 160, 200, 2);
    tft.drawString(String(aux_raw), 160, 200, 2);
    tft.drawString("      ", 240, 132, 2);
    tft.drawString(String((iris_get/100.0)), 240, 132, 2);
    tft.drawString("      ", 240, 149, 2);
    tft.drawString(String(zoom_get), 240, 149, 2);
    tft.drawString("          ", 240, 166, 2);
    tft.drawString(String(focus_get), 240, 166, 2);
}
void updateSub2() {
  
}
void updateSub3() {
  
}
void updateSub4() {
  
}
void updateSub5() {
  
}

void readSwitches() {
ret =  !PCF.read(13);
prod =  !PCF.read(15);
eng =  !PCF.read(14);
tbd =  !PCF.read(12);
}

void writeLEDs() {
  PCF.write(0,!call);
  PCF.write(1,!iso);
  PCF.write(2,!preview);
  PCF.write(3,!tally);
}

void updateOverview() {
  if(submenu==0){
    tft.setTextSize(1);
    tft.drawString("        ", IRIS_SET_X_POS, IRIS_SET_Y_POS, 2);
    tft.drawString(String(iris_set), IRIS_SET_X_POS, IRIS_SET_Y_POS, 2);

    tft.drawString("        ", IRIS_GET_X_POS, IRIS_GET_Y_POS, 2);
    tft.drawString(String(iris_get), IRIS_GET_X_POS, IRIS_GET_Y_POS, 2);

    tft.drawString("        ", FOCUS_SET_X_POS, FOCUS_SET_Y_POS, 2);
    tft.drawString(String(focus_set), FOCUS_SET_X_POS, FOCUS_SET_Y_POS, 2);

    tft.drawString("        ", FOCUS_GET_X_POS, FOCUS_GET_Y_POS, 2);
    tft.drawString(String(focus_get), FOCUS_GET_X_POS, FOCUS_GET_Y_POS, 2);

    tft.drawString("        ", ZOOM_SET_X_POS, ZOOM_SET_Y_POS, 2);
    tft.drawString(String(zoom_set), ZOOM_SET_X_POS, ZOOM_SET_Y_POS, 2);

    tft.drawString("        ", ZOOM_GET_X_POS, ZOOM_GET_Y_POS, 2);
    tft.drawString(String(zoom_get), ZOOM_GET_X_POS, ZOOM_GET_Y_POS, 2);

    tft.drawString("  ", BIT_1_X_POS, BIT_1_Y_POS, 2);
    tft.drawString(String(prod), BIT_1_X_POS, BIT_1_Y_POS, 2);

    tft.drawString("  ", BIT_2_X_POS, BIT_2_Y_POS, 2);
    tft.drawString(String(eng), BIT_2_X_POS, BIT_2_Y_POS, 2);

    tft.drawString("  ", BIT_3_X_POS, BIT_3_Y_POS, 2);
    tft.drawString(String(ret), BIT_3_X_POS, BIT_3_Y_POS, 2);

    tft.drawString("  ", BIT_4_X_POS, BIT_4_Y_POS, 2);
    tft.drawString(String(tbd), BIT_4_X_POS, BIT_4_Y_POS, 2);

    tft.drawString("  ", BIT_5_X_POS, BIT_5_Y_POS, 2);
    tft.drawString(String(aux), BIT_5_X_POS, BIT_5_Y_POS, 2);
  }
}

void loop() {
  bool pressed = tft.getTouch(&x, &y);
  if (pressed) {
    changeMenu();
  }
  switch (menu) {
    case 0:
      break;

    case 1:
      if(millis() >= lastUpdate+updateRate) {
        switch(submenu) {
          case(0):
          updateOverview();
          break;
          case(1):
          updateSub1();
          break;
          case(2):
          updateSub2();
          break;
          case(3):
          updateSub3();
          break;
          case(4):
          updateSub4();
          break;
          case(5):
          updateSub5();
          break;
        }        
      lastUpdate = millis();
      }
      break;

    case 2:
      if(millis() >= lastUpdate+updateRate) {
        updateVisual();
        lastUpdate = millis();
      }
    break;

    case 3:
      break;
  }
  readSwitches();
  if(tbd){
    ledDemo();
    generateDummyValues();
  } else {
    readAnalog();
    tally=0; 
    iso=0; 
    preview=0; 
    call = 0; 
    rgbLedWrite(21,0,0,0);
    writeLEDs();
  }
}