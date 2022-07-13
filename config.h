// ---------------------------> Time constants
#define AVG_RESET_TIME 4
#define RESET_TIME 7200
#define SECS 1000
#define JAMMED_TIME 5
#define WIFI_TIMEOUT 5000

// ---------------------------> Code flags
#define TEST true // <---------------------------> Change this to false for 2.9" screen
#define SENSOR_PIN 13
#define ALARM_PIN 14
#define BUFFER_SIZE 15

// ---------------------------> E-Paper variables
#define Y_COUNT 40
#define Y_PROM 110
#define L_PADDING 13
#define X_STATUS 150

#if TEST
  #define X_LABELS 10
#else
  #define X_LABELS 20
#endif
