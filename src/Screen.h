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
  void MQTTState(bool connected = false);
  void WiFiState(bool connected = false);
  void WorkersState(uint8_t no_workers = 0);
  void updateCount(const unsigned int value);
private:
};
#endif
