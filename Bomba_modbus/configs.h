#ifndef config_h
#define config_h

// ---------- Controller ------------
// Pinout
// Ultrasonic sensors configs
#define triggerPIN PB15
#define N_SENSORS  2
// Pump pins
#define pumpPIN         PC13
#define capacitorPIN    PB9

// Tanks
#define UPPERTANK  0
#define LOWERTANK  1

// Control
#define START_CAP_DELAY 3000 // ms
#define PUMP_ON_DELAY   5*60*1000 // ms (5min)
#define FULL_TANK 90

// ----------- Modbus ---------------
#define MB_SPEED 19200

// ----------- HMI ------------------
// Pinout
// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS PA4
#define TFT_RST PA2 // you can also connect this to the Arduino reset
// in which case, set this #define pin to 0!
#define TFT_DC PA3

#define b2    64
#define b1    b2 / 2
#define h1    50
#define c1    b1 / 2
#define h2    40
#define d     8
#define p1x   2
#define p1y   160-100
#define p2x   2
#define p2y   160-2
#define p3x   48
#define p3y   160-81
#define din   16
#define dout  32
#define h0    160-8

#endif // !config_h