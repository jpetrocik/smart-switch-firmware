#include <Arduino.h>
#include "configuration.h"
#include "relay.h"

void Relay::operateRelay(RELAY_STATE state)
{
    digitalWrite(pin, state);
}

void Relay::toogleRelay()
{
    RELAY_STATE currentRelayState = relayState();
    if (currentRelayState == RELAY_CLOSED)
    {
        openRelay();
    }
    else
    {
        closeRelay();
    }
}

void Relay::openRelay()
{
    RELAY_STATE currentRelayState = relayState();
    if (currentRelayState == RELAY_OPEN)
        return;

    operateRelay(RELAY_OPEN);
}

void Relay::closeRelay()
{
    RELAY_STATE currentRelayState = relayState();
    if (currentRelayState == RELAY_CLOSED)
        return;

    operateRelay(RELAY_CLOSED);
}

RELAY_STATE Relay::relayState()
{
    return (RELAY_STATE)digitalRead(pin);
}

void Relay::loop()
{
    if (debounceTime < millis())
    {
        prevState = state;

        state = relayState();
        if (prevState != state)
        {
            debounceTime = 50 + millis();
            stateChangehandler(state);
        }
    }
}

void Relay::begin(uint8_t realyPin)
{
    pin = realyPin;

    pinMode(pin, OUTPUT);
    digitalWrite(pin, RELAY_OPEN);
    state = relayState();
}

void Relay::setStateChangedHandler(RelayStateChangeHandler handler)
{
    stateChangehandler = handler;
}