// ---------------------------> Time constants
#define AVG_RESET_TIME 4
#define RESET_TIME 7200
#define RESET_HOUR 2
#define SECS 1000
#define JAMMED_TIME 5
#define WIFI_TIMEOUT 5000

#define TIME_ZONE_OFFSET_HRS            (-7)  /* Ensenada, México */
// #define TIME_ZONE_OFFSET_HRS            (+8)   /* Taiping, Malaysia */

// ---------------------------> Code flags
#define TEST true // <---------------------------> Change this to false for 2.9" screen
#define SENSOR_PIN 13
#define ALARM_PIN 14
#define BUFFER_SIZE 15

// ---------------------------> E-Paper variables
#define Y_COUNT 50
#define Y_PROM 160
#define L_PADDING 35
#define X_STATUS 20

#if TEST
  #define X_LABELS 10
#else
  #define X_LABELS 20
#endif
