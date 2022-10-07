#include "MqttCLient.h"
#include "secrets.h"

WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);

// void subscribeReceive(char* topic, byte* payload, unsigned int length);

void MqttClient::connect(){
  mqttClient.setServer(IP_ADDRESS, PORT);
  if (mqttClient.connect("myClientID","admin","midna")) {
    Serial.println("Connection has been established, well done");
    // subscribeRoutine();
     no_service_available = false;
  }
  else {
    Serial.println("Looks like the server connection failed...");
  }
}

bool MqttClient::isServiceAvailable(){
  return !no_service_available;
}

void MqttClient::reconnect(){
   while (!mqttClient.connected()) {
     Serial.print("Attempting MQTT connection...");
     String clientId = "ESP32Client-";
     clientId += String(random(0xffff), HEX);
     if (mqttClient.connect(clientId.c_str())) {
       Serial.println("connected");
      //  mqttClient.subscribe(DATA_TOPIC);
     } else {
       Serial.print("failed, rc=");
       Serial.print(mqttClient.state());
       Serial.println(" try again in 5 seconds");
       delay(5000);
     }
   }
}

bool MqttClient::isConnected(){
  return mqttClient.connected();
}

void MqttClient::loop(){
  if (!mqttClient.connected()) {
    reconnect();
  }
  delay(100);
  mqttClient.loop();
}

PubSubClient MqttClient::getRef(){
  return mqttClient;
}

void MqttClient::setCallback(std::function<void (char *, uint8_t *, unsigned int)> callback){
  mqttClient.setCallback(callback);
}
//
// void subscribeReceive(char* topic, byte* payload, unsigned int length){
//   Serial.print("Topic: ");
//   Serial.println(topic);
//   Serial.print("Message: ");
//   for(int i = 0; i < length; i ++){
//     Serial.print(char(payload[i]));
//   }
//   Serial.println("");
// }
//
void MqttClient::sendTagData(const char* id){
  if (no_service_available) return;
  if (mqttClient.publish(DATA_TOPIC, id)){
    Serial.println("Publish message success");
  }
  else{
    Serial.println("Could not send message :(");
    reconnect();
    // mqttClient.publish(DATA_TOPIC, id);
  }
}

 bool MqttClient::refreshMQTTStatus(){
  const bool connection = isConnected();
  if (connection != last_connection_state){
    last_connection_state = connection;
    return true;
  }
  return false;
 }

 bool MqttClient::getConnectionStatus(){
  return last_connection_state;
}
