#ifndef SWITCH_H
#define SWITCH_H

#include <Button2.h>
#include "relay.h"

enum DOOR_STATE
{
    DOOR_CLOSED,
    DOOR_OPEN
};

typedef void (*LongClickHandler)(Button2 &);

typedef void (*DoorStateChangeHandler)(DOOR_STATE state);

class Switch
{
protected:
    Relay relay;
    Button2 button;
    DoorStateChangeHandler doorStateChangeHandler;

public:
    void setStateChangedHandler(DoorStateChangeHandler handler);
    void setupLongClickHandler(LongClickHandler longPressHandler, LongClickHandler longReleaseHandler);
    void closeDoor();
    void openDoor();
    void toggleDoor();
    DOOR_STATE state();
    void setup(uint8_t buttonPin, uint8_t relayPin, uint8_t reedPin);
    void loop();
};

#endif