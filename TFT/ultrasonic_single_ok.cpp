#include "SR04.h"

#define trigger PB15 // Pin for the trigger signal (setted to timer 3)
#define echo1 PA8    // Pin for the echo signal

#define PULSEWIDTH 10        // microseconds
#define OVERFLOW_VALUE 65000 // 65ms cycle
#define PRESCALER_VALUE 72   // 1us clock

volatile int val, oldCount;
volatile int echo1width;
volatile bool newDistance1;

// We'll use timer 3
HardwareTimer timerX(3);

MedianFilter mf1 = MedianFilter();

int getLevel(){
    return mf1.getValue()/58;
}

void readSensors()
{
    // Pass a value to the median filter
    if (newDistance1)
    {
        mf1.insertValue(echo1width);
        newDistance1 = false;
    }
}

void setupTimers()
{
    // Set up the LED to blink
    pinMode(trigger, OUTPUT);
    pinMode(echo1, INPUT);
    attachInterrupt(echo1, exti_handler, CHANGE);

    // Pause the timer while we're configuring it
    timerX.pause();

    // Set up an interrupt on channel 1
    timerX.setChannel1Mode(TIMER_OUTPUT_COMPARE);
    timerX.setCompare(TIMER_CH1, PULSEWIDTH);  // Interrupt 10 count after each update
    timerX.setPrescaleFactor(PRESCALER_VALUE); // 1MHz
    timerX.setOverflow(OVERFLOW_VALUE);
    timerX.attachCompare1Interrupt(handler_trigger_off); // When compare
    timerX.attachInterrupt(0, handler_trigger_on);       // When over flow
    timerX.refresh();

    // Start the timer counting
    timerX.resume();
}

void handler_trigger_on()
{
    digitalWrite(trigger, HIGH);
}

void handler_trigger_off()
{
    digitalWrite(trigger, LOW);
}

void exti_handler()
{
    timerX.pause();

    val = digitalRead(echo1);
    if (val == HIGH)
    {
        oldCount = timerX.getCount();
    }
    else
    {
        echo1width = timerX.getCount() - oldCount;
        newDistance1 = true;
    }

    timerX.resume();
}
