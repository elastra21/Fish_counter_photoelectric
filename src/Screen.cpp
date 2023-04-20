#include "utility/In_eSPI.h"
// #include <sys/_stdint.h>
#include <M5Tough.h>
#include "Screen.h"

void Screen::init() {
}

void Screen::drawLabels() {
  WiFiState();
  MQTTState();
  WorkersState();
  M5.Lcd.setBrightness(20);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setFreeFont(&FreeSansBold12pt7b);
  M5.Lcd.drawString("Count:", X_LABELS, 50);
  M5.Lcd.drawString("FPM:", X_LABELS, 150);
  M5.Lcd.setFreeFont();
}

void Screen::printJam() {
  M5.Lcd.setTextSize(1);
  M5.Lcd.setFreeFont(&FreeSansBold24pt7b);
  M5.Lcd.drawString("JAM!   ", X_STATUS, Y_PROM + L_PADDING);
  M5.Lcd.setFreeFont();
}

void Screen::WiFiState(bool connected) {
  M5.Lcd.setTextSize(1);
  M5.Lcd.setFreeFont(&FreeSansBold9pt7b);
  const uint32_t background_color = connected ? TFT_GREEN : TFT_RED;
  const uint32_t text_color = connected ? TFT_BLACK : TFT_WHITE;
  M5.Lcd.fillRoundRect(5, 5, 80, 30, 3, background_color);
  // M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(text_color, background_color);
  M5.Lcd.drawString("WIFI", 25, 13);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setFreeFont();
}

void Screen::MQTTState(bool connected) {
  M5.Lcd.setTextSize(1);
  M5.Lcd.setFreeFont(&FreeSansBold9pt7b);
  const uint32_t background_color = connected ? TFT_GREEN : TFT_RED;
  const uint32_t text_color = connected ? TFT_BLACK : TFT_WHITE;
  M5.Lcd.fillRoundRect(100, 5, 80, 30, 3, background_color);
  // M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(text_color, background_color);
  M5.Lcd.drawString("MQTT", 115, 13);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setFreeFont();
}

void Screen::WorkersState(uint8_t no_workers) {
  const uint32_t background_color = TFT_YELLOW;
  const uint32_t text_color = TFT_BLACK;

  // M5.Lcd.setTextColor(text_color, background_color);
  // M5.Lcd.fillRoundRect(195, 5, 80, 30, 3, background_color);
  // M5.Lcd.setTextSize(1);
  // M5.Lcd.drawString("Workers", 215, 25);
  // M5.Lcd.setTextSize(2);
  // M5.Lcd.setTextDatum(TC_DATUM);
  // M5.Lcd.drawString(String(no_workers), 235, 8);
  // M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  // M5.Lcd.setTextDatum(TL_DATUM);
  
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextDatum(TR_DATUM);
  M5.Lcd.setFreeFont(&FreeSansBold12pt7b);
  M5.Lcd.drawString("Workers", 300, 150);
  M5.Lcd.setFreeFont(&FreeSansBold24pt7b);
  // M5.Lcd.setCursor(X_STATUS, Y_COUNT + L_PADDING);
  M5.Lcd.drawString(" "+String(no_workers), 300, Y_PROM + L_PADDING);
  M5.Lcd.setFreeFont();
  M5.Lcd.setTextDatum(TL_DATUM);
}

void Screen::resetAvg() {
  M5.Lcd.setTextSize(1);
  M5.Lcd.setFreeFont(&FreeSansBold24pt7b);
  M5.Lcd.drawString("0           ", X_STATUS, Y_PROM + L_PADDING);
  M5.Lcd.setFreeFont();
}

void Screen::updateProm(float avg) {
  M5.Lcd.setTextSize(1);
  M5.Lcd.setFreeFont(&FreeSansBold24pt7b);
  // M5.Lcd.setCursor(X_STATUS, Y_PROM + L_PADDING);
  M5.Lcd.drawString(String(avg < 1000 ? avg : 0) + "  ", X_STATUS, Y_PROM + L_PADDING);
  M5.Lcd.setFreeFont();
}

void Screen::updateCount(const unsigned int value) {
  M5.Lcd.setTextSize(1);
  M5.Lcd.setFreeFont(&FreeSansBold24pt7b);
  // M5.Lcd.setCursor(X_STATUS, Y_COUNT + L_PADDING);
  M5.Lcd.drawString(String(value) + "  ", X_STATUS, Y_COUNT + L_PADDING);
  M5.Lcd.setFreeFont();
}