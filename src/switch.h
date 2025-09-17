#ifndef SWITCH_H
#define SWITCH_H

#include <Button2.h>
#include "relay.h"

class Switch : public Button2
{
protected:
    Relay relay;

    void buttonReleasedHandler();

public:
    void setStateChangedHandler(RelayStateChangeHandler handler);
    void turnOff();
    void turnOn();
    RELAY_STATE state();
    void setup(uint8_t buttonPin, uint8_t relayPin);
    void Xloop();
};

#endif