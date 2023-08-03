#ifndef MY_WIFI_H
#define MY_WIFI_H
#include "EEPROM.h"
#include "secrets.h"
#include <ESPmDNS.h>
#include <WiFiMulti.h>
#include <ArduinoOTA.h>
#include "ESPAsyncWebServer.h"

class WIFI {
public:
  void localIP();
  void loopOTA();
  void setUpOTA();
  void setUpWiFi();
  void reconnect();
  bool isConnected();
  void setUpWebServer();
  bool refreshWiFiStatus();
  bool getConnectionStatus();

private:
  bool last_connection_state = false;
};
#endif
