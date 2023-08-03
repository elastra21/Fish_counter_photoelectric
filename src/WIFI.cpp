#include "WIFI.h"
#include "WebSite.h"
#include "WiFiType.h"

AsyncWebServer server(80);

static void handle_update_progress_cb(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (!index){
    int cmd = (filename.indexOf("spiffs") > -1) ? U_SPIFFS : U_FLASH;
    // Update.runAsync(true);
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
      Update.printError(Serial);
    }
  }

  if (Update.write(data, len) != len) {
    Update.printError(Serial);
  }

  if (final) {
    if (!Update.end(true)){
      Update.printError(Serial);
    }
  }
}

void WIFI::setUpWebServer(){
  /*use mdns for host name resolution*/
  if (!MDNS.begin("el-fish-counter")){ // http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1){
      delay(1000);
    }
  }
  Serial.println("mDNS responder started Pinche Hugo");
  /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200,"text/html", index_page);
    // server.send(200, "text/html", loginIndex); 
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200,"text/css", style);
  });

  server.on("/serverIndex", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200,"text/html", server_index);
  });
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []( AsyncWebServerRequest *request) {
    request->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart(); 
  },handle_update_progress_cb);

  server.begin();
}

void WIFI::setUpWiFi() {
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  uint32_t notConnectedCounter = 0;
  EEPROM.begin(32);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.println("Wifi connecting...");

    notConnectedCounter++;
    if (notConnectedCounter > 150) {  // Reset board if not connected after 5s
      Serial.println("Resetting due to Wifi not connecting...");
      const uint8_t num_of_tries = EEPROM.readInt(1);
      if (num_of_tries == 3) break;
      else {
        EEPROM.writeInt(1, num_of_tries + 1);
        EEPROM.commit();
        EEPROM.end();
        ESP.restart();
      }
    }
  }

  EEPROM.writeInt(1, 0);
  EEPROM.commit();
  EEPROM.end();

  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void WIFI::setUpOTA() {
  if (isConnected()) {
    ArduinoOTA.setHostname(HOST_NAME);
    ArduinoOTA.onStart([]() {
                String type;
                type = ArduinoOTA.getCommand() == U_FLASH ? "sketch" : "filesystem";
                // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
                Serial.println("Start updating " + type);
              })
      .onEnd([]() {
        Serial.println("\nEnd");
      })
      .onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      })
      .onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
      });
    ArduinoOTA.begin();
  }
}

void WIFI::loopOTA() {
  ArduinoOTA.handle();
}

bool WIFI::refreshWiFiStatus() {
  const bool connection = isConnected();
  if (connection != last_connection_state) {
    last_connection_state = connection;
    return true;
  }
  return false;
}

bool WIFI::getConnectionStatus() {
  return last_connection_state;
}

bool WIFI::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void WIFI::reconnect() {
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  uint8_t timeout = 0;
  vTaskDelay(2000);
  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(2000);
    log_i(" waiting on wifi connection");
    timeout++;
    if (timeout == 2) return;
  }
}
