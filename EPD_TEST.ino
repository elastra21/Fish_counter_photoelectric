
#include "config.h"
#include <GxEPD2_BW.h> // including both doesn't use more code or ram
#include <GxEPD2_3C.h> // including both doesn't use more code or ram
#include <U8g2_for_Adafruit_GFX.h>


#if TEST
  GxEPD2_BW<GxEPD2_270, GxEPD2_270::HEIGHT> display(GxEPD2_270(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEW027W3, Waveshare 2.7"
#else
  GxEPD2_BW<GxEPD2_290_T94_V2, GxEPD2_290_T94_V2::HEIGHT> display(GxEPD2_290_T94_V2(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEM029T94, Waveshare 2.9" V2 variant
#endif

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

void updateText();

////////////////////////////////
volatile unsigned long fishCounter = 0;
volatile boolean should_update_count = false;
volatile boolean isUp = false;
volatile unsigned long startTime = 0.0;
volatile unsigned long endTime = 0.0;
volatile unsigned long duration = 1;
volatile unsigned long next_reset = 0;


unsigned int time_buffer[BUFFER_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
boolean full_buffer = false;
uint8_t buffer_position = 0;
boolean jammed = false;
unsigned long jam_countdown = 0;
unsigned long next_avg_reset = 0;

void updateProm();

////////////////////////////////

void IRAM_ATTR handleInterrupt(){
  isUp = !isUp;
  if (isUp) {
    if (startTime > 0) {
      endTime = millis();
      duration = (endTime - startTime);
    }
    fishCounter++;
    startTime = millis();
    should_update_count = true;
    setNextReset();
  }
}

void setup(){
  Serial.begin(115200);
  pinMode(ALARM_PIN, OUTPUT);
  initScreen();
  drawLabels();
  attachInterrupt(SENSOR_PIN, handleInterrupt, CHANGE);
  setNextReset();
}

void loop(){
  if (should_update_count) updateCount(fishCounter);

  if (next_avg_reset < millis()) resetAvg();

  if (next_reset < millis()) reset_values();

  if (jam_countdown != 0 && jam_countdown < millis())
    if (digitalRead(SENSOR_PIN)== LOW) isJamed();
}

// ================================= TODO MOVE SCREEN STUFF TO ANOTHER CLASS =================================

void initScreen(){

     display.init();
     display.setTextColor(GxEPD_BLACK);
     display.firstPage();
     display.setRotation(1);
     u8g2Fonts.begin(display);
     delay(1000);
     u8g2Fonts.setForegroundColor(GxEPD_BLACK);
     u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
}
// --------------------------------------------------------> MOVE TO SCREEN CLASS

void drawLabels(){

     do{
       display.fillScreen(GxEPD_WHITE);
       u8g2Fonts.setFont(u8g2_font_fub30_tr);   //font is set
       u8g2Fonts.setCursor(X_LABELS, Y_COUNT);             //cursor(x,y)
       u8g2Fonts.print("Count: ");   //print the text
       u8g2Fonts.setCursor(X_LABELS, Y_PROM);             //cursor(x,y)
       u8g2Fonts.print("FPM: ");   //print the text
       u8g2Fonts.setCursor(X_STATUS + L_PADDING, Y_COUNT);
       u8g2Fonts.print(0);
       u8g2Fonts.setCursor(X_STATUS + L_PADDING, Y_PROM);
       u8g2Fonts.print(0);
     }while (display.nextPage());
}    // --------------------------------------------------------> MOVE TO SCREEN CLASS

void updateCount(const unsigned int value){
  resetJamTime();
  jam_countdown = JAMMED_TIME * SECS + millis();
  should_update_count = false;
  display.setPartialWindow(X_STATUS, 0, 200, 70);
  updateText(X_STATUS + L_PADDING, Y_COUNT, String(value).c_str());
  if (duration > 0 ) {
    // const float promedio = 60000/duration;
    // updateProm(promedio < 1000 ? promedio: 0);
    updateProm();
    next_avg_reset = AVG_RESET_TIME * SECS + millis();
  }
} // ----------------------------------> MOVE TO SCREEN CLASS

void updateProm(){
  if (jammed || duration < 100) return;
  time_buffer[buffer_position] = duration;
  const boolean position_reset = buffer_position + 1 > BUFFER_SIZE;
  if (!full_buffer && position_reset) full_buffer = true;
  buffer_position = position_reset ? 0 : buffer_position + 1;
  unsigned int sum = 0;
  for ( int i = 0; i < BUFFER_SIZE; i++ )
    sum += time_buffer[ i ];
  const uint8_t buffer_lenght = full_buffer ? BUFFER_SIZE : buffer_position;
  const float avg = sum > 1 ? (buffer_lenght * 60000)/ sum : 0;
  display.setPartialWindow(X_STATUS, 70, 200, 120);
  updateText(X_STATUS + L_PADDING, Y_PROM, String(avg < 1000 ? avg: 0).c_str());
} // ---------------------------------------------------> MOVE TO SCREEN CLASS

void updateText(const uint16_t x, const uint16_t y, const char *text){
    display.firstPage();
    do{
      display.fillScreen(GxEPD_WHITE);
      u8g2Fonts.setCursor(x, y);
      u8g2Fonts.print(text);
    }while(display.nextPage());
} // --------> MOVE TO SCREEN CLASS

void setNextReset(){
  next_reset = RESET_TIME * SECS + millis();
}

void reset_values(){
  // fishCounter = 0;
  // resetAvg();
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
  for (size_t i = 0; i < BUFFER_SIZE; i++)
    time_buffer[i] = 0;
  buffer_position = 0;
  display.setPartialWindow(X_STATUS, 70, 200, 120);
  updateText(X_STATUS + L_PADDING, Y_PROM, "0");
}

void isJamed(){
  if (!jammed) {
    jammed = true;
    digitalWrite(ALARM_PIN, HIGH);
    u8g2Fonts.setFont(u8g2_font_fub20_tr);   //font is set
    display.setPartialWindow(X_STATUS, 70, 200, 120);
    updateText(X_STATUS + L_PADDING, Y_PROM, "JAMMED");
    u8g2Fonts.setFont(u8g2_font_fub30_tr);   //font is set
  }
}
