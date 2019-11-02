#include <EEPROM.h>

int ledPin =  PC13;    // LED connected to digital pin 13
const char HELP_MSG[] = "Press :\r\n" \
			" w address value - Write value in address\r\n" \			
      " r address       - Read the value in address\r\n";
uint16 DataWrite;
uint16 AddressWrite;

void setup()
{
	// initialize the digital pin as an output:
	pinMode(ledPin, OUTPUT);
  Serial.begin();
  // wait for serial monitor to be connected.
  while (!Serial)
  {
    digitalWrite(33,!digitalRead(33));// Turn the LED from off to on, or on to off
    delay(100);         // fast blink
  }
	Serial.print(HELP_MSG);
  EEPROM.PageBase0 = 0x801F000;
  EEPROM.PageBase1 = 0x801F800;
  EEPROM.PageSize  = 0x400;
}

void loop()
{
	uint16 Status;
	uint16 Data;

	while (Serial.available())
	{
		int cmd = Serial.read();
		Serial.println(cmd);
		if (cmd == (int)'h')
		{
			Serial.printlnt(HELP_MSG);
		}
	}
}
