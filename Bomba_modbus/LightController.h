#ifndef light_h
#define light_h

#include "configs.h"

#define MAX_ANALOG_READ 4095
#define NC_VALUE 200

#define THRESHOLD_HYST 5
#define MIN2MSEC 60000 // Convert minutes to milliseconds

// Light Modes
enum modes
{
    ACTIVE,   // Automatic light control (Keep as first mode!!!)
    ON,       // Always ON
    COURTESY, // Turns on under a threshold luminosity value
    INACTIVE, // Always OFF (Keep as last mode!!!)
};

// Light FSM states
enum light_states
{
    SLEEPING,    // Stays OFF all nigth  (Keep as first state!!!)
    ACTIVE_OFF,  // Ligth OFF IDLE state
    ACTIVE_ON,   // Ligth ON
    ACTIVE_OFF1, // Ligth OFF watching for smart delay increase/reset (Keep as last state!!!)
};

#define MIN_SLEEP_TIME 0
#define MAX_SLEEP_TIME 900 // 15hrs
#define MIN_MODE ACTIVE
#define MAX_MODE INACTIVE
#define MIN_SMART_DELAY 10      // seconds
#define MAX_SMART_DELAY 30      // seconds
#define MIN_INIT_DELAY 30       // seconds
#define MAX_INIT_DELAY 900      // seconds
#define MIN_DELAY_INCREMENT 30  // seconds
#define MAX_DELAY_INCREMENT 900 // seconds
#define MIN_THRESHOLD 0         // %
#define MAX_THRESHOLD 100       // %

class LightController
{

public:
    LightController();
    void init(int relayPin, int relayOn, int pirPin, int pirOn,
              int luminosityPin, int luminosityMonotony,
              int addrMode, int addrSleepTime, int addrSmart, int addrSmartDelay,
              int addrInitDelay, int addrDelayIncrement, int addrThreshold),
        init(int relayPin, int pirPin,
             int luminosityPin,
             int addrMode, int addrSleepTime, int addrSmart, int addrSmartDelay,
             int addrInitDelay, int addrDelayIncrement, int addrThreshold),
        run(),
        startSleep();

    // getters
    int getLight_state(),
        getPir_state(),
        getLuminosity(),
        getMode(),
        getSleepTime(),
        getSmart(),
        getSmartDelay(),
        getInitDelay(),
        getDelayIncrement(),
        getThreshold();

    // setters
    int setMode(int newvalue),
        setSleepTime(int newvalue),
        setSmart(int newvalue),
        setSmartDelay(int newvalue),
        setInitDelay(int newvalue),
        setDelayIncrement(int newvalue),
        setThreshold(int newvalue);

private:
    void config(int relayPin, int addrMode, int addrSleepTime, int addrSmart,
                int addrSmartDelay, int addrInitDelay, int addrDelayIncrement,
                int addrThreshold);
    int relay_pin,           // Output pin controlling the light
        relay_on,            // Active value of the pin controlling the light
        pir_pin,             // Input pin of the PIR sensor (-1 means NC)
        pir_on,              // Active value of the PIR sensor
        luminosity_pin,      // Input pin of the luminosity sensor (-1 means NC)
        luminosity_monotony, // > 0 implies that higher value indicates higher luminosity (<0 otherwise)
        light_state,         // ON/OFF
        luminosity,          // Current luminosity value % (-1 means sensor NC)
        // ---------- Addresses -------------
        addr_mode,
        addr_sleepTime,
        addr_smart,
        addr_smartDelay,
        addr_initDelay,
        addr_delayIncrement,
        addr_threshold,
        // ----------------------------------
        sleepTime,           // Time for leaving sleep state after night (min)
        smartDelay,          // Delay to watch for movements after light turns off (seconds)
        initDelay,           // Inital delay for light to be on movement detected (min)
        delayOn,             // Current delay for light to be on movement detected (min)
        delayIncrement,      // Amount of time to be added to the initial delay on smart lights
        luminosityThreshold; // Luminosity threshold for turning light on

    bool smart,        // Smart behaviour of light
        pir_state,     // PIR sensor state (ON/OFF)
        lastSatisfied; // Threshold was previously satisfied

    modes mode; // Current light mode

    light_states currentState; // FSM state

    uint32 timeStamp;

    void scanSensors(),
        turnOnLight(),
        turnOffLight(),
        handleActiveMode(),
        handleCourtesyMode();

    bool thresholdSatisfied();
};

#endif // light_h
