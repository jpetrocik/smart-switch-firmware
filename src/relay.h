#ifndef LIGHT_H
#define LIGHT_H

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

class Relay
{
    typedef void (*StateChangeHandler)(RELAY_STATE state);

protected:
    uint8_t pin;
    RELAY_STATE state;
    RELAY_STATE prevState;
    unsigned long debounceTime;
    StateChangeHandler stateChangehandler;
    void operateRelay(RELAY_STATE state);

public:
    void begin(uint8_t relayPin);
    void setChangedHandler(StateChangeHandler handler);
    void loop();
    void toogleRelay();
    void openRelay();
    void closeRelay();
    RELAY_STATE relayState();
};

#endif