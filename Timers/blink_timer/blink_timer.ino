#define trigger PB15

int toggle = 0;

// We'll use timer 2
HardwareTimer timer(3);

void setup() {
    // Set up the LED to blink
    pinMode(trigger, OUTPUT);

    // Pause the timer while we're configuring it
    timer.pause();

    // Set up period
    //timer.setPeriod(LED_RATE); // in microseconds

    // Set up an interrupt on channel 1
    timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
    timer.setCompare(TIMER_CH1, 10);  // Interrupt 10 count after each update
    timer.setPrescaleFactor(72); // 1MHz
    timer.setOverflow(65000);  // Interrupt 1 count after each update
    timer.attachCompare1Interrupt(handler_led_off);    
    timer.attachInterrupt(0,handler_led_on); // over flow

    // Refresh the timer's count, prescale, and overflow
    timer.refresh();

    // Start the timer counting
    timer.resume();
}

void loop() {
    // Nothing! It's all in the handler_led() interrupt:
}

void handler_led(void) {
    toggle ^= 1;
    digitalWrite(trigger, toggle);
}
void handler_led_on(void) {
    digitalWrite(trigger, HIGH);
}
void handler_led_off(void) {
    digitalWrite(trigger, LOW);
}
