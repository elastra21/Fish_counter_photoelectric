#include "Screen.h"

void Screen::init(){

}

void Screen::drawLabels(){
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(X_LABELS, 20);
  M5.Lcd.print("Count:");
  M5.Lcd.setCursor(X_LABELS, 120);
  M5.Lcd.print("FPM:");
}  

void Screen::printJam(){
  M5.Lcd.setTextSize(5);
  M5.Lcd.setCursor(X_STATUS, Y_PROM + L_PADDING);
  M5.Lcd.print("JAM!");
}

void Screen::resetAvg(){
  M5.Lcd.setTextSize(5);
  M5.Lcd.setCursor(X_STATUS, Y_PROM + L_PADDING);
  M5.Lcd.print("0               ");
}

void Screen::updateProm(float avg){
  M5.Lcd.setTextSize(5);
  M5.Lcd.setCursor(X_STATUS, Y_PROM + L_PADDING);
  M5.Lcd.print(String(avg < 1000 ? avg: 0).c_str());
}

void Screen::updateCount(const unsigned int value){
  M5.Lcd.setTextSize(5);
  M5.Lcd.setCursor(X_STATUS, Y_COUNT + L_PADDING);
  M5.Lcd.print(String(value).c_str()); 
}