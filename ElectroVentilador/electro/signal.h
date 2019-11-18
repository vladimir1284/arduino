#include "configs.h"

// Screens
enum signal_states
{
    ACTIVE,
    INACTIVE,
    WAIT
};

class ExternalSignal
{

public:
    ExternalSignal();

    void run(); // Main method to be refreshed in every loop
    void init(int pin);
    bool isActive();

private:
    int digitalPIN;
    unsigned int startWait;
    signal_states estado;
};
