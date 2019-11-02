#include <Arduino.h>
#line 1 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/primeros_ejemplos/Thermocouple/Thermocouple.ino"
#line 1 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/primeros_ejemplos/Thermocouple/Thermocouple.ino"
// #include <MAX6675_Thermocouple.h>

// #define SCK_PIN 46
// #define CS_PIN  45
// #define SO_PIN  43

// MAX6675_Thermocouple* thermocouple = NULL;

// void setup() {
//   // wait for MAX chip to stabilize
//   thermocouple = new MAX6675_Thermocouple(SCK_PIN, CS_PIN, SO_PIN);
//   delay(500);
// }

// void loop() {
//   // basic readout test, just print the current temp

//   double celsius = thermocouple->readCelsius();
//   delay(100);
// }

// this example is public domain. enjoy!
// www.ladyada.net/learn/sensors/thermocouple

#include "max6675.h"

int thermoDO = PB7;
int thermoCS = PB8;
int thermoCLK = PB9;

float temp;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
// int vccPin = 3;
// int gndPin = 2;

void setup()
{
  // Serial.begin(9600);
  // // use Arduino pins
  // pinMode(vccPin, OUTPUT); digitalWrite(vccPin, HIGH);
  // pinMode(gndPin, OUTPUT); digitalWrite(gndPin, LOW);

  Serial.println("MAX6675 test");
  // wait for MAX chip to stabilize
  delay(500);
}

void loop()
{
  int i;
  // basic readout test, just print the current temp
  temp = 0;
  for (i = 0; i < 100; i++)
  {
    temp += thermocouple.readCelsius();
  }
  temp /= 100;
  delay(500);
}
