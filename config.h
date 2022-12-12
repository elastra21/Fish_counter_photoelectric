// ---------------------------> Time constants
#define AVG_RESET_TIME 4
#define RESET_TIME 7200
#define RESET_HOUR 2
#define SECS 1000
#define JAMMED_TIME 5
#define WIFI_TIMEOUT 5000

#define TIME_ZONE_OFFSET_HRS (-7) /* Ensenada, México */
// #define TIME_ZONE_OFFSET_HRS            (+8)   /* Taiping, Malaysia */

// ---------------------------> Code flags
#define TEST true  // <---------------------------> Change this to false for 2.9" screen

#define SENSOR_PIN    13
#define ALARM_PIN     14
#define VALVE         0 
#define ADD_WORKER    25
#define DEL_WORKER    26
#define FLUSH_VALVE   27 

#define MAX_WORKERS   6

#define FLUSH_TIME  1800      /* SECONDS */
#define FLUSH_PROCESS 180     /* SECONDS */
#define WATER_VALVE_TIME 15   /* SECONDS */

// #define Q0_2  26  // Stage #3
// #define Q0_4  19  // Fan

// #define A0    13 //ONE_WIRE_BUS

// #define I0_10 34  // Stop button
// #define I0_12 2  // Start button

// #define A0_5  27  //                            *
// #define I0_11 33  // Delayed start button       *

#define BUFFER_SIZE   15

// ---------------------------> E-Paper variables
#define Y_COUNT       50
#define Y_PROM        150
#define L_PADDING     35
#define X_STATUS      20

#if TEST
#define X_LABELS      10
#else
#define X_LABELS      20
#endif
