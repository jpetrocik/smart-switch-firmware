#include <Arduino.h>
#include "switch.h"
#include "configuration.h"

void Switch::setup()
{
    highButton.begin(HIGH_BUTTON_PIN, INPUT_PULLUP);
    highButton.setClickHandler([this](Button2 &btn)
                               { this->turnOnHigh(); });

    highRelay.begin(HIGH_RELY_PIN);
    highRelay.setStateChangedHandler([this](RELAY_STATE state)
                                     {     
                                        if (state == RELAY_CLOSED) {
                                            switchStateChangeHandler(SWITCH_HIGH);
                                     } });

    lowButton.begin(LOW_BUTTON_PIN, INPUT_PULLUP);
    lowButton.setClickHandler([this](Button2 &btn)
                              { this->turnOnLow(); });

    lowRelay.begin(LOW_RELY_PIN);
    lowRelay.setStateChangedHandler([this](RELAY_STATE state)
                                     {     
                                        if (state == RELAY_CLOSED) {
                                            switchStateChangeHandler(SWITCH_LOW);
                                     } });

    offButton.begin(OFF_BUTTON_PIN, INPUT_PULLUP);
    offButton.setClickHandler([this](Button2 &btn)
                              { this->turnOff(); });
}

void Switch::loop()
{
    highButton.loop();
    highRelay.loop();

    lowButton.loop();
    lowRelay.loop();

    offButton.loop();
}

void Switch::setStateChangedHandler(SwitchStateChangeHandler handler)
{
    switchStateChangeHandler = handler;
}

void Switch::turnOnHigh()
{
    if (highRelay.relayState() == RELAY_CLOSED)
        return;

    turnOff();
    delay(50);
    highRelay.closeRelay();
}

void Switch::turnOnLow()
{
    if (lowRelay.relayState() == RELAY_CLOSED)
        return;

    turnOff();
    delay(50);
    lowRelay.closeRelay();
}

void Switch::turnOff()
{
    highRelay.openRelay();
    lowRelay.openRelay();
    switchStateChangeHandler(SWITCH_OFF);
}

SWITCH_STATE Switch::state()
{
    if (highRelay.relayState() == RELAY_CLOSED)
        return SWITCH_HIGH;

    if (lowRelay.relayState() == RELAY_CLOSED)
        return SWITCH_LOW;

    return SWITCH_OFF;
}

void Switch::setupLongClickHandler(LongClickHandler longPressButtonHandler, LongClickHandler longReleaseButtonHandler)
{
    offButton.setLongClickTime(2000);
    offButton.setLongClickDetectedRetriggerable(true);
    offButton.setLongClickDetectedHandler(longPressButtonHandler);
    offButton.setLongClickHandler(longReleaseButtonHandler);
}
