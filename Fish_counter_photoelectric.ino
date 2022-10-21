#include "WIFI.h"
#include "Screen.h"
#include "config.h"
#include "secrets.h"
#include <WiFiUdp.h>
#include <WiFiMulti.h>
#include "MqttClient.h"
#include <NTPClient_Generic.h> // Sí

WIFI wifi;
Screen screen;
WiFiUDP ntpUDP;
MqttClient mqtt;
TaskHandle_t Task1;
NTPClient timeClient(ntpUDP);
RTC_DateTypeDef RTC_DateStruct; // Data
RTC_TimeTypeDef RTC_TimeStruct; // Time

void isJamed();
void setUpRTC();
void resetAvg();
void updateProm();
void resetValues();
void setNextReset();
void resetJamTime();
void sendToMQTT(String count, String avg);
void backgroundTasks( void * pvParameters );
void updateText(const uint16_t x, const uint16_t y, const char *text);

////////////////////////////////
bool jammed = false;
char timeStrbuff[64];
bool full_buffer = false;
volatile bool is_up = false;
uint8_t buffer_position = 0;
unsigned long jam_countdown = 0;
unsigned long next_avg_reset = 0;
volatile unsigned long duration = 1;
volatile unsigned long end_time = 0;
volatile unsigned long next_reset = 0;
volatile unsigned long start_time = 0;
volatile unsigned long fish_counter = 0;
volatile bool should_update_count = false;
unsigned int time_buffer[BUFFER_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
////////////////////////////////

void IRAM_ATTR handleInterrupt() {
  is_up = !is_up;
  if (is_up) {
    if (start_time > 0) {
      end_time = millis();
      duration = (end_time - start_time);
    }
    fish_counter++;
    start_time = millis();
    should_update_count = true;
    setNextReset();
  }
}

void setup(){
  M5.begin();
  Serial.begin(115200);

  pinMode(ALARM_PIN, OUTPUT);
  
  screen.init();
  screen.drawLabels();
  screen.updateCount(0);

  attachInterrupt(SENSOR_PIN, handleInterrupt, CHANGE);
  setNextReset();
  xTaskCreatePinnedToCore(backgroundTasks, "Task1", 10000,  NULL, 1, &Task1, CORE0);
}

void backgroundTasks( void * pvParameters ){
  wifi.setUpWiFi();
  if(wifi.isConnected()) setUpRTC();
  wifi.setUpOTA();
  mqtt.connect();
  for(;;){
    wifi.isConnected() ? wifi.loopOTA(): wifi.reconnect();
    if (mqtt.isServiceAvailable())  mqtt.loop();
  }
}

void loop(){ 
  M5.Rtc.GetTime(&RTC_TimeStruct);

  if (wifi.refreshWiFiStatus())   screen.WiFiState(wifi.getConnectionStatus());
  
  if (mqtt.refreshMQTTStatus())   screen.MQTTState(mqtt.getConnectionStatus());

  if (RTC_TimeStruct.Hours == RESET_HOUR) resetValues();
  
  if (should_update_count)        updateCount(fish_counter);

  if (next_avg_reset < millis())  resetAvg();

  // if (next_reset < millis())      resetValues();

  if (jam_countdown != 0 && jam_countdown < millis())
    if (digitalRead(SENSOR_PIN)== HIGH) isJamed();
}

void updateCount(const unsigned int value){
  resetJamTime();
  jam_countdown = JAMMED_TIME * SECS + millis();
  should_update_count = false;
  screen.updateCount(value);
  updateProm();
  next_avg_reset = AVG_RESET_TIME * SECS + millis();
} // ----------------------------------> MOVE TO SCREEN CLASS

void updateProm(){
  if (jammed || duration < 100) return;
  time_buffer[buffer_position] = duration;
  const boolean position_reset = buffer_position + 1 > BUFFER_SIZE;
  if (!full_buffer && position_reset) full_buffer = true;
  buffer_position = position_reset ? 0 : buffer_position + 1;
  unsigned int sum = 0;
  for ( int i = 0; i < BUFFER_SIZE; i++ ) sum += time_buffer[ i ];
  const uint8_t buffer_lenght = full_buffer ? BUFFER_SIZE : buffer_position;
  const float avg = sum > 1 ? (buffer_lenght * 60000)/ sum : 0;
  screen.updateProm(avg);
  sendToMQTT(String(fish_counter),String(avg));
} 

void setNextReset(){
  next_reset = RESET_TIME * SECS + millis();
}

void sendToMQTT(String count, String avg){
  if (wifi.isConnected() && mqtt.isConnected()){
    const String msg = "{\"count\":"+count+", \"fpm\":"+avg+"}";
    mqtt.sendTagData(msg.c_str());
  }
}

void resetValues(){
  fish_counter = 0;
  resetAvg();
}

void resetJamTime(){
  jam_countdown = 0;
  if (jammed) {
    jammed = false;
    digitalWrite(ALARM_PIN, LOW);
  }
}

void resetAvg(){
  if (jammed) return;
  full_buffer = false;
  for (size_t i = 0; i < BUFFER_SIZE; i++) time_buffer[i] = 0;
  buffer_position = 0;
  screen.resetAvg();
  next_avg_reset = AVG_RESET_TIME * SECS + millis();
  sendToMQTT(String(fish_counter),"0");
}

void isJamed(){
  if (!jammed) {
    jammed = true;
    digitalWrite(ALARM_PIN, HIGH);
    screen.printJam();
    sendToMQTT(String(fish_counter),"-1");
  }
}

void setUpRTC(){
  timeClient.begin();
  timeClient.setTimeOffset(3600 * TIME_ZONE_OFFSET_HRS);
  timeClient.setUpdateInterval(SECS_IN_HR);
  timeClient.update();
  while (!timeClient.updated()){
    timeClient.update();
    delay(500);
  }
  RTC_DateStruct.Year     = timeClient.getYear();
  RTC_DateStruct.Month    = timeClient.getMonth();
  RTC_DateStruct.Date     = timeClient.getDay();
  M5.Rtc.SetDate(&RTC_DateStruct);
  RTC_TimeStruct.Hours    = timeClient.getHours();
  RTC_TimeStruct.Minutes  = timeClient.getMinutes();
  RTC_TimeStruct.Seconds  = timeClient.getSeconds();
  M5.Rtc.SetTime(&RTC_TimeStruct); 
}
