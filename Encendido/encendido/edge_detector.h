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
    bool detect_edge(bool rising, int &count);

protected:
    bool state;
    int count_r;
    int count_f;
    int pin;
};
#endif