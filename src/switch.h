#ifndef SWITCH_H
#define SWITCH_H

#include <Button2.h>
#include "relay.h"

enum SWITCH_STATE
{
    SWITCH_OFF,
    SWITCH_ON
};

typedef void (*LongClickHandler)(Button2 &);

typedef void (*SwitchStateChangeHandler)(SWITCH_STATE state);

class Switch
{
protected:
    Relay relay;
    Button2 button;
    SwitchStateChangeHandler switchStateChangeHandler;

public:
    void setStateChangedHandler(SwitchStateChangeHandler handler);
    void setupLongClickHandler(LongClickHandler longPressHandler, LongClickHandler longReleaseHandler);
    void turnOff();
    void turnOn();
    void toggle();
    SWITCH_STATE state();
    void setup(uint8_t buttonPin, uint8_t relayPin);
    void loop();
};

#endif