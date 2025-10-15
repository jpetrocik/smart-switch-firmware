#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "device.h"
#include "configuration.h"
#include "ArduinoJson.h"

static char jsonStatusBuffer[250];

DeviceConfig deviceConfig;

DeviceState deviceState = {
    .switchState = SWITCH_OFF};

DeviceConfig *currentDeviceConfig()
{
    return &deviceConfig;
}

DeviceState *currentDeviceState()
{
    return &deviceState;
}

const char *currentDeviceStateJson()
{
    JsonDocument jsonDoc;
    jsonDoc["state"] = deviceState.switchState == SWITCH_ON ? "ON" : "OFF";
    jsonDoc["status"] = deviceState.switchState;
    jsonDoc["chipId"] = ESP.getChipId();

    char ipBuf[16];
    IPAddress ip = WiFi.localIP();
    snprintf(ipBuf, sizeof(ipBuf), "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
    jsonDoc["ipAddress"] = ipBuf;

    jsonDoc["rssi"] = WiFi.RSSI();

    size_t len = serializeJson(jsonDoc, jsonStatusBuffer, sizeof(jsonStatusBuffer));
    if (len >= sizeof(jsonStatusBuffer))
    {
        jsonStatusBuffer[sizeof(jsonStatusBuffer) - 1] = '\0';
    }

    return jsonStatusBuffer;
}

const char *currentDeviceConfigJson()
{
    JsonDocument jsonDoc;
    jsonDoc["device"] = deviceConfig.deviceName;
    jsonDoc["room"] = deviceConfig.roomName;
    jsonDoc["location"] = deviceConfig.locationName;
    jsonDoc["mqttHost"] = deviceConfig.mqttHost;
    jsonDoc["wifiSsid"] = deviceConfig.wifiSsid;
    jsonDoc["wifiPassword"] = deviceConfig.wifiPassword;
    jsonDoc["disableLed"] = deviceConfig.disableLed;

    size_t len = serializeJson(jsonDoc, jsonStatusBuffer, sizeof(jsonStatusBuffer));
    if (len >= sizeof(jsonStatusBuffer))
    {
        jsonStatusBuffer[sizeof(jsonStatusBuffer) - 1] = '\0';
    }

    return jsonStatusBuffer;
}