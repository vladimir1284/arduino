#define PULSEWIDTH 10 // microseconds
#define OVERFLOW_VALUE 65000  // 65ms cycle
#define PRESCALER_VALUE 72  // 1us clock

#include "Arduino.h"

#define trigger PB15  // Pin for the trigger signal (settetd to timer3)

class SR04 {

 public:
  SR04();

 private:
   void setupTimers();
   // We'll use timer 3
   HardwareTimer timer3;
};


void handler_trigger_on() {
    digitalWrite(trigger, 1);
}

void handler_trigger_off() {
    digitalWrite(trigger, 0);
}
