#include <Arduino.h>
#include "configuration.h"
#include "relay.h"

void operateDoor(RELAY_STATE state)
{
    digitalWrite(RELAY_PIN, state);
}

void toogleRelay()
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

void openRelay()
{
    RELAY_STATE currentRelayState = relayState();
    if (currentRelayState == RELAY_OPEN)
        return;

    operateDoor(RELAY_OPEN);
}

void closeRelay()
{
    RELAY_STATE currentRelayState = relayState();
    if (currentRelayState == RELAY_CLOSED)
        return;

    operateDoor(RELAY_CLOSED);
}

RELAY_STATE relayState()
{
    return (RELAY_STATE)digitalRead(RELAY_PIN);
}
