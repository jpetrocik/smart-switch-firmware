#include <Arduino.h>
#include "switch.h"
#include "configuration.h"

void Switch::setup(uint8_t buttonPin, uint8_t relayPin)
{
    button.begin(buttonPin, INPUT);
    button.setLongClickTime(1000);
    button.setClickHandler([this](Button2 &btn)
                           { this->toggle(); });

    relay.begin(relayPin);
    relay.setStateChangedHandler([this](RELAY_STATE state)
                                 { this->switchStateChangeHandler(state == RELAY_CLOSED ? SWITCH_ON : SWITCH_OFF); });
}

void Switch::loop()
{
    button.loop();
    relay.loop();
}

void Switch::setStateChangedHandler(SwitchStateChangeHandler handler)
{
    switchStateChangeHandler = handler;
}

void Switch::toggle()
{
    relay.toogleRelay();
}

void Switch::turnOff()
{
    relay.openRelay();
}

void Switch::turnOn()
{
    relay.closeRelay();
}

SWITCH_STATE Switch::state()
{
    return relay.relayState() == RELAY_CLOSED ? SWITCH_ON : SWITCH_OFF;
}

void Switch::setupLongClickHandler(LongClickHandler longPressButtonHandler, LongClickHandler longReleaseButtonHandler)
{
    button.setLongClickTime(2000);
    button.setLongClickDetectedRetriggerable(true);
    button.setLongClickDetectedHandler(longPressButtonHandler);
    button.setLongClickHandler(longReleaseButtonHandler);
}
