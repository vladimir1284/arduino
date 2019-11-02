#include "SR04.h"

void SR04::SR04() {  
    timer3 = HardwareTimer(3);
    setupTimers();
    
}

void SR04::setupTimers() {

    // Pause the timer while we're configuring it
    timer3.pause();

    // Set up an interrupt on channel 1
    timer3.setChannel1Mode(TIMER_OUTPUT_COMPARE);
    timer3.setCompare(TIMER_CH1, PULSEWIDTH);  // Interrupt 10 count after each update
    timer3.setPrescaleFactor(PRESCALER_VALUE); // 1MHz
    timer3.setOverflow(OVERFLOW_VALUE);  
    timer3.attachCompare1Interrupt(handler_trigger_off); // When the comparison value is reached
    timer3.attachInterrupt(0,handler_trigger_on); // When over flow
    timer3.refresh();

    // Start the timer counting
    timer3.resume();
}

