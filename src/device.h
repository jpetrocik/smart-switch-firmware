#ifndef DEVICE_H
#define DEVICE_H
#include "configuration.h"
#include <ArduinoJson.h>
#include "switch.h"

struct DeviceConfig
{
    char deviceName[20] = "light";
    char roomName[20] = "room";
    char locationName[20] = "house";
    char hostname[41] = "room-light";
    char mqttHost[50];
    char wifiSsid[25];
    char wifiPassword[50];
    bool disableLed = false;
    bool dirty = false;
};

DeviceConfig *currentDeviceConfig();
const char *currentDeviceConfigJson();
const char * currentDeviceStateJson();

//Device function to be implemented in main.cpp
void switchTurnOn();
void switchTurnOff();
void switchToggle();
SWITCH_STATE switchState();
void deviceRestart();
void deviceFactoryReset();
#endif