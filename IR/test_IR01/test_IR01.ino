
#include <irmp.h>

void setup() {
  timer2 = HardwareTimer(2);
  // Pause the timer while we're configuring it
  timer2.pause();

  timer2.setPrescaleFactor(599); // 120MHz
  timer2.setOverflow(7); // 15 kSamples/s
  timer2.attachInterrupt(0,handler_IR); // When over flow
  timer2.refresh();

  // Start the timer counting
  timer2.resume();

  Serial.begin();  // USB does not require BAUD
  // wait for serial monitor to be connected.
  while (!Serial)
  {
  }
}

void loop() {
  IRMP_DATA* data = &irmp_data[act_data];
  if (irmp_get_data(data))
    {
        #if IRMP_PROTOCOL_NAMES == 1
        Serial.print(irmp_protocol_names[data->protocol]);
        Serial.print(" ");
        #endif
        Serial.print("P:");
        Serial.print(data->protocol, HEX);
        Serial.print(" A:");
        Serial.print(data->address, HEX);
        Serial.print(" C:");
        Serial.print(data->command, HEX);
        Serial.print(" ");
        Serial.print(data->flags, HEX);
        Serial.println("");

        data->flags = 0;    // reset flags!
        
  }

void handler_IR(){
  irmp_ISR();

}

