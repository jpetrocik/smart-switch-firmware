#ifndef RELAY_H
#define RELAY_H

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

typedef void (*RelayStateChangeHandler)(RELAY_STATE state);

class Relay
{
protected:
    uint8_t pin;
    RELAY_STATE state;
    RELAY_STATE prevState;
    unsigned long debounceTime;
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