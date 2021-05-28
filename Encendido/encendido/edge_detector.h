#ifndef edge_h
#define edge_h

#include <Arduino.h>

#define NSAMPLES 5

class EdgeDetector
{

public:
    EdgeDetector(int pin);
    bool detect_rising();
    bool detect_falling();

private:
    bool detect_edge(bool rising);

protected:
    bool state;
    int count;
    int pin;
};
#endif