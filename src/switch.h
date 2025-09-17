#ifndef SWITCH_H
#define SWITCH_H

#include <Button2.h>
#include "relay.h"

enum SWICH_STATE
{
    SWITCH_OFF,
    SWITCH_LOW,
    SWITCH_HIGH
};

typedef void (*LongClickHandler)(Button2 &);
typedef void (*SwitchStateChangeHandler)(SWICH_STATE state);

class Switch
{
protected:
    Relay highRelay;
    Relay lowRelay;
    Button2 highButton;
    Button2 lowButton;
    Button2 offButton;
    SwitchStateChangeHandler switchStateChangeHandler;

    void buttonReleasedHandler();

public:
    void setStateChangedHandler(SwitchStateChangeHandler handler);
    void setupLongClickHandler(LongClickHandler longPressHandler, LongClickHandler longReleaseHandler);
    void turnOff();
    void turnOnHigh();
    void turnOnLow();
    SWICH_STATE state();
    void setup();
    void Xloop();
};

#endif