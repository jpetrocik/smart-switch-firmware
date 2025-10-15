#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "device.h"
#include "configuration.h"
#include "ArduinoJson.h"

static char jsonStatusBuffer[250];

DeviceConfig deviceConfig;

DeviceState deviceState = {
    .relayState = RELAY_OPEN
};

DeviceConfig *currentDeviceConfig()
{
    return &deviceConfig;
}

DeviceState *currentDeviceState()
{
    return &deviceState;
}

const char *currentDeviceStatusJson()
{
    StaticJsonDocument<250> jsonDoc;
    jsonDoc["state"] = deviceState.relayState == RELAY_CLOSED ? "ON" : "OFF";
    jsonDoc["status"] = deviceState.relayState;
    jsonDoc["chipId"] = ESP.getChipId();
    jsonDoc["ipAddress"] = WiFi.localIP().toString();
    jsonDoc["rssi"] = WiFi.RSSI();

    serializeJson(jsonDoc, jsonStatusBuffer, sizeof(jsonStatusBuffer));
    return jsonStatusBuffer;
}

const char* currentDeviceConfigJson()
{
    StaticJsonDocument<250> jsonDoc;
    jsonDoc["device"] = deviceConfig.deviceName;
    jsonDoc["room"] = deviceConfig.roomName;
    jsonDoc["location"] = deviceConfig.locationName;
    jsonDoc["mqttHost"] = deviceConfig.mqttHost;
    jsonDoc["wifiSsid"] = deviceConfig.wifiSsid;
    jsonDoc["wifiPassword"] = deviceConfig.wifiPassword;
    jsonDoc["disableLed"] = deviceConfig.disableLed;

    serializeJson(jsonDoc, jsonStatusBuffer, sizeof(jsonStatusBuffer));
    return jsonStatusBuffer;
}