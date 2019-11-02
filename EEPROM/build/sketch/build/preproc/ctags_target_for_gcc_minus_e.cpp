# 1 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Proyects/EEPROM/testEEPROM.ino"
# 1 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Proyects/EEPROM/testEEPROM.ino"
# 2 "/home/vladimir/Documentos/SOFTWARE/arduino/BluePill/Proyects/EEPROM/testEEPROM.ino" 2

int ledPin = PC13; // LED connected to digital pin 13
const char HELP_MSG[] = "Press :\r\n"
   " 0 display configuration\r\n"
   " 1 set configuration to 0x801F000 / 0x801F800 / 0x400 (RB MCU)\r\n"
   " 2 set configuration to 0x801F000 / 0x801F800 / 0x800 (ZE/RE MCU)\r\n"
   " 3 write/read variable\r\n"
   " 4 increment address\r\n"
   " 5 display pages top/bottom\r\n"
   " 6 initialize EEPROM\r\n"
   " 7 format EEPROM\r\n";
uint16 DataWrite = 38;
uint16 AddressWrite = 0;

void setup()
{
 uint16 Status;
 uint16 Data;

 // Configure
  EEPROM.PageBase0 = 0x801F000;
  EEPROM.PageBase1 = 0x801F800;
  EEPROM.PageSize = 0x400;

  // Read
  //Status = EEPROM.read(EEPROM.PageBase0+AddressWrite, &Data);
  // Write 
  Status = EEPROM.write(EEPROM.PageBase1+AddressWrite, DataWrite);
  // Read
  Status = EEPROM.read(EEPROM.PageBase1+AddressWrite, &Data);
  delay(5);
}

void loop(){

}
