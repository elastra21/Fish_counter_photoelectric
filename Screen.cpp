#include "M5Tough.h"
#include "Screen.h"

void Screen::init(){

}

void Screen::drawLabels(){
  WiFiState();
  MQTTState();
  M5.Lcd.setBrightness(20);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(X_LABELS, 50);
  M5.Lcd.print("Count:");
  M5.Lcd.setCursor(X_LABELS, 160);
  M5.Lcd.print("FPM:");
}  

void Screen::printJam(){
  M5.Lcd.setTextSize(5);
  M5.Lcd.setCursor(X_STATUS, Y_PROM + L_PADDING);
  M5.Lcd.print("JAM!");
}

void Screen::WiFiState(bool connected){
  const uint32_t background_color = connected ? TFT_GREEN : TFT_RED; 
  const uint32_t text_color = connected ? TFT_BLACK : TFT_WHITE;
  M5.Lcd.fillRoundRect(5, 5, 80, 30, 3, background_color);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(text_color, background_color);
  M5.Lcd.drawString("WiFi",25,13);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
}

void Screen::MQTTState(bool connected){
  const uint32_t background_color = connected ? TFT_GREEN : TFT_RED; 
  const uint32_t text_color = connected ? TFT_BLACK : TFT_WHITE;
  M5.Lcd.fillRoundRect(100, 5, 80, 30, 3, background_color);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(text_color, background_color);
  M5.Lcd.drawString("MQTT",120,13);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
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