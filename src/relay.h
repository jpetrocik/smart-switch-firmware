#ifndef RELAY_H
#define RELAY_H

#include "configuration.h"

#ifdef RELAY_INVERTED
enum RELAY_STATE
{
    RELAY_CLOSED,
    RELAY_OPEN
};
#else
enum RELAY_STATE
{
    RELAY_OPEN,
    RELAY_CLOSED
};
#endif

typedef std::function<void(RELAY_STATE state)> RelayStateChangeHandler;

class Relay
{
protected:
    uint8_t pin;
    RELAY_STATE state;
    RELAY_STATE prevState;
    unsigned long debounceTime = 0;
    RelayStateChangeHandler stateChangehandler;
    void operateRelay(RELAY_STATE state);

public:
    void begin(uint8_t relayPin);
    void setStateChangedHandler(RelayStateChangeHandler handler);
    void loop();
    void toogleRelay();
    void openRelay();
    void closeRelay();
    RELAY_STATE relayState();
};

#endif