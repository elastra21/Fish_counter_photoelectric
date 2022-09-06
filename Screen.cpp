#include "Screen.h"

#if TEST
  GxEPD2_BW<GxEPD2_270, GxEPD2_270::HEIGHT> display(GxEPD2_270(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEW027W3, Waveshare 2.7"
#else
  GxEPD2_BW<GxEPD2_290_T94_V2, GxEPD2_290_T94_V2::HEIGHT> display(GxEPD2_290_T94_V2(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEM029T94, Waveshare 2.9" V2 variant
#endif

void Screen::init(){
  display.init();
  display.setTextColor(GxEPD_BLACK);
  display.firstPage();
  display.setRotation(1);
  u8g2Fonts.begin(display);
  delay(1000);
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);
  u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
}

void Screen::drawLabels(){
  do{
    display.fillScreen(GxEPD_WHITE);
    u8g2Fonts.setFont(u8g2_font_fub30_tr);   //font is set
    u8g2Fonts.setCursor(X_LABELS, Y_COUNT);             //cursor(x,y)
    u8g2Fonts.print("Count: ");   //print the text
    u8g2Fonts.setCursor(X_LABELS, Y_PROM);             //cursor(x,y)
    u8g2Fonts.print("FPM: ");   //print the text
    u8g2Fonts.setCursor(X_STATUS + L_PADDING, Y_COUNT);
    u8g2Fonts.print(0);
    u8g2Fonts.setCursor(X_STATUS + L_PADDING, Y_PROM);
    u8g2Fonts.print(0);
  }while (display.nextPage());
}  

void Screen::printJam(){
  u8g2Fonts.setFont(u8g2_font_fub30_tr);   //font is set
  display.setPartialWindow(X_STATUS, 70, 200, 120);
  updateText(X_STATUS + L_PADDING, Y_PROM, "JAM!");
  // sendToMQTT(String(fish_counter),"-1");
  u8g2Fonts.setFont(u8g2_font_fub30_tr);   //font is set
}

void Screen::resetAvg(){
  display.setPartialWindow(X_STATUS, 70, 200, 120);
  updateText(X_STATUS + L_PADDING, Y_PROM, "0");
  // sendToMQTT(String(fish_counter),"0");
}

void Screen::updateText(const uint16_t x, const uint16_t y, const char *text){
  display.firstPage();
  do{
    display.fillScreen(GxEPD_WHITE);
    u8g2Fonts.setCursor(x, y);
    u8g2Fonts.print(text);
  } while(display.nextPage());
}

void Screen::updateProm(float avg){
  display.setPartialWindow(X_STATUS, 70, 200, 120);
  updateText(X_STATUS + L_PADDING, Y_PROM, String(avg < 1000 ? avg: 0).c_str());   
}

void Screen::updateCount(const unsigned int value){
  display.setPartialWindow(X_STATUS, 0, 200, 70);
  updateText(X_STATUS + L_PADDING, Y_COUNT, String(value).c_str());
}