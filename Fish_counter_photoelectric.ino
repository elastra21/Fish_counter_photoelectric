#include "WIFI.h"
#include "Screen.h"
#include "config.h"
#include "secrets.h"
#include <WiFiUdp.h>
#include <WiFiMulti.h>
#include "MqttClient.h"
#include <NTPClient_Generic.h>  // Sí

WIFI wifi;
Screen screen;
WiFiUDP ntpUDP;
MqttClient mqtt;
TaskHandle_t Task1;
NTPClient timeClient(ntpUDP);
RTC_DateTypeDef RTC_DateStruct;  // Data
RTC_TimeTypeDef RTC_TimeStruct;  // Time

void isJamed();
void setUpRTC();
void resetAvg();
void addWorker();
void updateProm();
void resetValues();
void setNextReset();
void resetJamTime();
void sustractWorker();
void resetFlushTime();
void resetOnWorkingWatchdog();
void setFlushValve(bool value);
void setOnWorkingValve(bool value);
void backgroundTasks(void* pvParameters);
void sendToMQTT(String count, String avg);
void updateText(const uint16_t x, const uint16_t y, const char* text);
void subscribeReceive(char* topic, byte* payload, unsigned int length);

////////////////////////////////
bool jammed = false;
bool full_buffer = false;
uint8_t worker_counter = 0;
uint32_t jam_countdown = 0;
bool flush_water_valve = 0;
uint32_t next_avg_reset = 0;
uint8_t buffer_position = 0;
bool last_add_state = false;
bool last_del_state = false;
volatile bool is_up = false;
volatile uint32_t duration = 1;
volatile uint32_t end_time = 0;
bool on_working_water_valve = 0;
volatile uint32_t next_reset = 0;
volatile uint32_t start_time = 0;
volatile uint32_t fish_counter = 0;
uint32_t flush_water_valve_timer = 0;
volatile bool should_update_count = false;
volatile uint32_t on_working_water_valve_watchdog = 0;
uint16_t time_buffer[BUFFER_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
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
    resetOnWorkingWatchdog();
  }
}

void setup() {
  M5.begin();
  Serial.begin(115200);

  pinMode(VALVE, OUTPUT);
  pinMode(ALARM_PIN, OUTPUT);

  pinMode(ADD_WORKER, INPUT);
  pinMode(DEL_WORKER, INPUT);

  screen.init();
  screen.drawLabels();
  screen.updateCount(0);

  attachInterrupt(SENSOR_PIN, handleInterrupt, CHANGE);
  setNextReset();
  resetFlushTime();
  xTaskCreatePinnedToCore(backgroundTasks, "Task1", 10000, NULL, 1, &Task1, CORE0);

  mqtt.setCallback(subscribeReceive);
}

void backgroundTasks(void* pvParameters) {
  wifi.setUpWiFi();
  if (wifi.isConnected()) setUpRTC();
  wifi.setUpOTA();
  mqtt.connect();
  for (;;) {
    wifi.isConnected() ? wifi.loopOTA() : wifi.reconnect();
    if (mqtt.isServiceAvailable()) mqtt.loop();
    delay(20);
  }
}

void loop() {
  M5.Rtc.GetTime(&RTC_TimeStruct);

  if (digitalRead(ADD_WORKER) == HIGH && !last_add_state) addWorker();
  else if (digitalRead(ADD_WORKER) == LOW && last_add_state) last_add_state = !last_add_state;

  if (digitalRead(DEL_WORKER) == HIGH && !last_del_state) sustractWorker();
  else if (digitalRead(DEL_WORKER) == LOW && last_del_state) last_del_state = !last_del_state;

  if (wifi.refreshWiFiStatus()) screen.WiFiState(wifi.getConnectionStatus());

  if (mqtt.refreshMQTTStatus()) screen.MQTTState(mqtt.getConnectionStatus());

  if (RTC_TimeStruct.Hours == RESET_HOUR) resetValues();

  if (should_update_count) updateCount(fish_counter);

  if (next_avg_reset < millis()) resetAvg();

  // if (next_reset < millis())      resetValues();

  if (jam_countdown != 0 && jam_countdown < millis())
    if (digitalRead(SENSOR_PIN) == HIGH) isJamed();

  if (on_working_water_valve_watchdog < millis() && on_working_water_valve) setOnWorkingValve(false);

  if (flush_water_valve_timer < millis()) setFlushValve(!flush_water_valve);
}

void updateCount(const uint16_t value) {
  resetJamTime();
  jam_countdown = JAMMED_TIME * SECS + millis();
  should_update_count = false;
  screen.updateCount(value);
  updateProm();
  next_avg_reset = AVG_RESET_TIME * SECS + millis();
}  // ----------------------------------> MOVE TO SCREEN CLASS

void updateProm() {
  if (jammed || duration < 100) return;
  time_buffer[buffer_position] = duration;
  const bool position_reset = buffer_position + 1 > BUFFER_SIZE;
  if (!full_buffer && position_reset) full_buffer = true;
  buffer_position = position_reset ? 0 : buffer_position + 1;
  uint16_t sum = 0;
  for (uint16_t i = 0; i < BUFFER_SIZE; i++) sum += time_buffer[i];
  const uint8_t buffer_lenght = full_buffer ? BUFFER_SIZE : buffer_position;
  const float avg = sum > 1 ? (buffer_lenght * 60000) / sum : 0;
  screen.updateProm(avg);
  sendToMQTT(String(fish_counter), String(avg));
}

void setNextReset() {
  next_reset = RESET_TIME * SECS + millis();
}

void sendToMQTT(String count, String avg) {
  if (wifi.isConnected() && mqtt.isConnected()) {
    const String msg = "{\"count\":" + count + ", \"fpm\":" + avg + ", \"workers\":" + worker_counter + "}";
    mqtt.sendTagData(msg.c_str());
  }
}

void resetValues() {
  fish_counter = 0;
  resetAvg();
}

void addWorker(){
  if (worker_counter == MAX_WORKERS) return;
  worker_counter++;
  screen.WorkersState(worker_counter);
  last_add_state = !last_add_state;
}

void sustractWorker(){
  if (worker_counter == 0) return;
  worker_counter--;  
  screen.WorkersState(worker_counter);
  last_del_state = !last_del_state;
}

void resetJamTime() {
  jam_countdown = 0;
  if (jammed) {
    jammed = false;
    digitalWrite(ALARM_PIN, LOW);
  }
}

void resetAvg() {
  if (jammed) return;
  full_buffer = false;
  for (size_t i = 0; i < BUFFER_SIZE; i++) time_buffer[i] = 0;
  buffer_position = 0;
  screen.resetAvg();
  next_avg_reset = AVG_RESET_TIME * SECS + millis();
  sendToMQTT(String(fish_counter), "0");
}

void isJamed() {
  if (!jammed) {
    jammed = true;
    digitalWrite(ALARM_PIN, HIGH);
    screen.printJam();
    sendToMQTT(String(fish_counter), "-1");
  }
}

void resetOnWorkingWatchdog() {
  if (!on_working_water_valve) setOnWorkingValve(true);
  on_working_water_valve_watchdog = WATER_VALVE_TIME + millis();
}

void setOnWorkingValve(bool value) {
  on_working_water_valve = value;
  digitalWrite(VALVE, value);
}

void resetFlushTime() {
  flush_water_valve_timer = FLUSH_TIME + millis();
}

void setFlushValve(bool value) {
  flush_water_valve = value;
  digitalWrite(FLUSH_VALVE, value);
  if (value) flush_water_valve_timer = FLUSH_PROCESS + millis();
  else resetFlushTime();
}

void setUpRTC() {
  timeClient.begin();
  timeClient.setTimeOffset(3600 * TIME_ZONE_OFFSET_HRS);
  timeClient.setUpdateInterval(SECS_IN_HR);
  timeClient.update();
  while (!timeClient.updated()) {
    timeClient.update();
    delay(500);
  }
  RTC_DateStruct.Year = timeClient.getYear();
  RTC_DateStruct.Month = timeClient.getMonth();
  RTC_DateStruct.Date = timeClient.getDay();
  M5.Rtc.SetDate(&RTC_DateStruct);
  RTC_TimeStruct.Hours = timeClient.getHours();
  RTC_TimeStruct.Minutes = timeClient.getMinutes();
  RTC_TimeStruct.Seconds = timeClient.getSeconds();
  M5.Rtc.SetTime(&RTC_TimeStruct);
  Serial.println("hay puto RTC");
}

void subscribeReceive(char* topic, byte* payload, unsigned int length) {
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print(char(payload[i]));
  }
  Serial.println("");
}
