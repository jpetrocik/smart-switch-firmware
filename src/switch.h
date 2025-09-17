#ifndef SWITCH_H
#define SWITCH_H

#include <Button2.h>
#include "relay.h"

typedef void (*LongClickHandler)(Button2 &);

class Switch
{
protected:
    Relay relay;
    Button2 button;

    void buttonReleasedHandler();

public:
    void setStateChangedHandler(RelayStateChangeHandler handler);
    void setupLongClickHandler(LongClickHandler longPressHandler, LongClickHandler longReleaseHandler);
    void turnOff();
    void turnOn();
    RELAY_STATE state();
    void setup(uint8_t buttonPin, uint8_t relayPin);
    void Xloop();
};

#endif