#include <Arduino.h>
#line 1 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Demos/SR04/test_ultrasonic/test_ultrasonic.ino"
#line 1 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Demos/SR04/test_ultrasonic/test_ultrasonic.ino"
#line 1 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Demos/SR04/test_ultrasonic/test_ultrasonic.ino"
void setup();
#line 7 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Demos/SR04/test_ultrasonic/test_ultrasonic.ino"
void loop();
#line 1 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Demos/SR04/test_ultrasonic/test_ultrasonic.ino"
void setup() {
  // initialize digital pin PC13 as an output.
  pinMode(PC13, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(PC13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);              // wait for a second
  digitalWrite(PC13, LOW);    // turn the LED off by making the voltage LOW
  delay(500);              // wait for a second
}
