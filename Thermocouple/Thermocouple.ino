 #include "max6675.h"

//  int thermoDO = PB7;
//  int thermoCS = PB8;
//  int thermoCLK = PB9;
 int thermoDO = PA8;
 int thermoCS = PA9;
 int thermoCLK = PA10;

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

   // Serial.println("MAX6675 test");
   // // wait for MAX chip to stabilize
   // delay(500);
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
