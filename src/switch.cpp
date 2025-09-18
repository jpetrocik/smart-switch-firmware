#include <Arduino.h>
#include "switch.h"
#include "configuration.h"

void Switch::setup(uint8_t buttonPin, uint8_t relayPin, uint8_t reedPin)
{
    button.begin(buttonPin, INPUT);
    button.setClickHandler([this](Button2 &btn)
                           { this->toggleDoor(); });

    relay.begin(relayPin, reedPin);
    relay.setStateChangedHandler([this](RELAY_STATE state)
                                 { this->doorStateChangeHandler(state == RELAY_CLOSED ? DOOR_CLOSED : DOOR_OPEN); });
}

void Switch::loop()
{
    button.loop();
    relay.loop();
}

void Switch::setStateChangedHandler(DoorStateChangeHandler handler)
{
    doorStateChangeHandler = handler;
}

void Switch::toggleDoor()
{
    relay.toogleRelay();
}

void Switch::closeDoor()
{
    relay.openRelay();
}

void Switch::openDoor()
{
    relay.closeRelay();
}

DOOR_STATE Switch::state()
{
return relay.relayState() == RELAY_CLOSED ? DOOR_CLOSED : DOOR_OPEN;
}

void Switch::setupLongClickHandler(LongClickHandler longPressButtonHandler, LongClickHandler longReleaseButtonHandler)
{
    button.setLongClickTime(2000);
    button.setLongClickDetectedRetriggerable(true);
    button.setLongClickDetectedHandler(longPressButtonHandler);
    button.setLongClickHandler(longReleaseButtonHandler);
}
