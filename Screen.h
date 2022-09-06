#ifndef MY_SCREEN_H
#define MY_SCREEN_H
#include "config.h"
#include <GxEPD2_BW.h> 
#include <GxEPD2_3C.h> 
#include <U8g2_for_Adafruit_GFX.h>

class Screen {
  public:
    void init();
    void printJam();
    void resetAvg();
    void drawLabels();
    void updateProm(float avg);
    void updateCount(const unsigned int value);
  private:
    U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;
    void updateText(const uint16_t x, const uint16_t y, const char *text);
};
#endif
