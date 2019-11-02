#include "SR04.h"

#define PULSEWIDTH 10        // microseconds
#define OVERFLOW_VALUE 65000 // 65ms cycle
#define PRESCALER_VALUE 72   // 1us clock

int *SENSOR_PINS; // Echo pins
int trigger, N_SENSORS;      // Pin for the trigger signal


volatile int lastVal[MAX_SENSORS],
    newValue[MAX_SENSORS],
    oldCount[MAX_SENSORS];

// We'll use timer 3
HardwareTimer timerX(3);

//MedianFilter mf1 = MedianFilter();
MedianFilter filters[MAX_SENSORS] = {MedianFilter(), MedianFilter()};

int getLevel(int sensor)
{
    return filters[sensor].getValue() / 58;
}

void readSR04sensors()
{
    int i;
    // Pass a value to the median filter
    for (i = 0; i < N_SENSORS; i++)
    {
        if (newValue[i])
        {
            filters[i].insertValue(newValue[i]);
            newValue[i] = 0;
        }
    }
}

void setupUltrasonicSensors(int trigerPin, int nSensors, int echoPins[])
{
    int i;
    // User configs
    trigger = trigerPin;
    SENSOR_PINS = echoPins;
    N_SENSORS = nSensors;

    // Set up the LED to blink
    pinMode(trigger, OUTPUT);

    // Init variables
    // Set up echo sensorPins
    for (i = 0; i < N_SENSORS; i++)
    {
        pinMode(SENSOR_PINS[i], INPUT);
        attachInterrupt(SENSOR_PINS[i], exti_handler, CHANGE);
        lastVal[i] = 0;
        newValue[i] = 0;
        oldCount[i] = 0;
    }

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
    int i, val;

    timerX.pause();

    for (i = 0; i < N_SENSORS; i++)
    {
        val = digitalRead(SENSOR_PINS[i]);
        if (lastVal[i] != val)
        { // This is the relevant pin
            lastVal[i] = val;
            if (val == HIGH)
            {
                oldCount[i] = timerX.getCount();
            }
            else
            {
                // Store new value
                newValue[i] = timerX.getCount() - oldCount[i];
            }
        }
    }
    timerX.resume();
}
