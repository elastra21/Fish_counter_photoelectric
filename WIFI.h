#ifndef MY_WIFI_H
#define MY_WIFI_H
#include "EEPROM.h"
#include "secrets.h"
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <WiFiMulti.h>
#include <ArduinoOTA.h>

class WIFI {
  public:
    void localIP();
    void loopOTA();
    void setUpOTA();
    void setUpWiFi();
};
#endif
