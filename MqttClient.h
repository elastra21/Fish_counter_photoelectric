#ifndef MY_MQTT_H
#define MY_MQTT_H
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

class MqttClient {
  public:
    bool isServiceAvailable();
    void connect();
    void reconnect();
    boolean isConnected();
    void loop();
    void sendTagData(const char*);
    PubSubClient getRef();
    void setCallback(std::function<void (char *, uint8_t *, unsigned int)> callback);
  private:
    bool no_service_available = true;
};
#endif
