#ifndef MY_SCREEN_H
#define MY_SCREEN_H
#include "config.h"
#include <M5Tough.h>

class Screen {
  public:
    void init();
    void printJam();
    void resetAvg();
    void drawLabels();
    void updateProm(float avg);
    void updateCount(const unsigned int value);
  private:
};
#endif
