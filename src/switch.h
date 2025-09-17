#ifndef SWITCH_H
#define SWITCH_H

#include <Button2.h>
#include "relay.h"

enum SWITCH_STATE
{
    SWITCH_OFF,
    SWITCH_LOW,
    SWITCH_HIGH
};

typedef void (*LongClickHandler)(Button2 &);
typedef void (*SwitchStateChangeHandler)(SWITCH_STATE state);

class Switch
{
protected:
    Relay highRelay;
    Relay lowRelay;
    Button2 highButton;
    Button2 lowButton;
    Button2 offButton;
    SwitchStateChangeHandler switchStateChangeHandler;

public:
    void setStateChangedHandler(SwitchStateChangeHandler handler);
    void setupLongClickHandler(LongClickHandler longPressHandler, LongClickHandler longReleaseHandler);
    void turnOff();
    void turnOnHigh();
    void turnOnLow();
    SWITCH_STATE state();
    void setup();
    void loop();
};

#endif