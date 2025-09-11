#ifndef LIGHT_H
#define LIGHT_H

#ifdef RELAY_INVERTED
enum RELAY_STATE
{
    RELAY_CLOSED,
    RELAY_OPEN
};
#else
enum RELAY_STATE
{
    RELAY_OPEN,
    RELAY_CLOSED
};
#endif

void toogleRelay();
void openRelay();
void closeRelay();
RELAY_STATE relayState();

#endif