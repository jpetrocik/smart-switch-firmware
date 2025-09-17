#include <Arduino.h>
#include "switch.h"
#include "configuration.h"

void Switch::setup(uint8_t buttonPin, uint8_t relayPin)
{
    begin(buttonPin, INPUT);
    setClickHandler([this](Button2 &btn)
                    { this->buttonReleasedHandler(); });

    relay.begin(relayPin);
}

void Switch::Xloop()
{
    loop();
    relay.loop();
}

void Switch::setStateChangedHandler(RelayStateChangeHandler handler)
{
    relay.setStateChangedHandler(handler);
}

void Switch::buttonReleasedHandler()
{
    relay.toogleRelay();
}

void Switch::turnOff() {
    relay.openRelay();
}

void Switch::turnOn() {
    relay.closeRelay();
}

RELAY_STATE Switch::state() {
    return relay.relayState();
}
