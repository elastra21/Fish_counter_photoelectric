#ifndef MY_MQTT_H
#define MY_MQTT_H
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// #define DATA_TOPIC "RFID"

class MqttClient {
public:
  void loop();
  void connect();
  void reconnect();
  bool isConnected();
  PubSubClient getRef();
  bool refreshMQTTStatus();
  bool isServiceAvailable();
  bool getConnectionStatus();
  void sendTagData(const char *);
  void setCallback(std::function<void(char *, uint8_t *, unsigned int)> callback);
private:
  bool no_service_available = true;
  bool last_connection_state = false;
};
#endif
