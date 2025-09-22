#include "configuration.h"

#ifdef MQTT_ENABLED
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include "mqtt.h"
#include "switch.h"

WiFiClient _espClient;
PubSubClient _mqClient(_espClient);

int _reconnectAttemptCounter = 0;
unsigned long _nextReconnectAttempt = 0;

char _commandTopic[100];
char _statusTopic[100];
char _lwtTopic[100];
char _jsonStatusBuffer[140];
DeviceConfig *_mqttDeviceConfig;
unsigned long lastStatePublishCounter = 0;
Switch *_mqttSwitch;

void mqttCallback(char *topic, byte *payload, unsigned int length);
void mqttConnect();

void mqttSetup(DeviceConfig *deviceConfig, Switch *switch1)
{
  _mqttDeviceConfig = deviceConfig;
  _mqttSwitch = switch1;

  if (strlen(_mqttDeviceConfig->mqttHost) == 0)
  {
    Serial.println("MQTT host not configured");
    return;
  }

  Serial.println("Connecting to MQTT Server....");
  _mqClient.setServer(_mqttDeviceConfig->mqttHost, 1883);
  _mqClient.setCallback(mqttCallback);
  //  _mqClient.setKeepAlive(120);

  sprintf(_commandTopic, "%s/%s/%s/command", _mqttDeviceConfig->locationName, _mqttDeviceConfig->roomName, _mqttDeviceConfig->deviceName);
  sprintf(_statusTopic, "%s/%s/%s/status", _mqttDeviceConfig->locationName, _mqttDeviceConfig->roomName, _mqttDeviceConfig->deviceName);
  sprintf(_lwtTopic, "%s/%s/%s/LWT", _mqttDeviceConfig->locationName, _mqttDeviceConfig->roomName, _mqttDeviceConfig->deviceName);
}

void mqttLoop()
{
  if (!_mqClient.connected())
  {
    mqttConnect();
  }
  else
  {
    _mqClient.loop();

    // Publish state every 5 minutes
    if (millis() > 5 * 60 * 1000 * lastStatePublishCounter)
    {
      mqttSendStatus();
      lastStatePublishCounter++;
    }
  }
}

void mqttConnect()
{

  if (strlen(_mqttDeviceConfig->mqttHost) == 0)
  {
    return;
  }

  if (!_mqClient.connected() && _nextReconnectAttempt < millis())
  {

    sprintf(_jsonStatusBuffer, CLIENT_ID, ESP.getChipId());
    if (_mqClient.connect(_jsonStatusBuffer, _lwtTopic, 0, true, "Offline"))
    {
      Serial.println("Connected to MQTT Server");
      _mqClient.publish(_lwtTopic, "Online", true);
      _mqClient.subscribe(_commandTopic);

      _reconnectAttemptCounter = 0;
      _nextReconnectAttempt = 0;
    }
    else
    {
      Serial.println("Failed to connect to MQTT Server");

      _reconnectAttemptCounter++;
      _nextReconnectAttempt = sq(_reconnectAttemptCounter) * 1000;
      if (_nextReconnectAttempt > 30000)
        _nextReconnectAttempt = 30000;

      Serial.print("Will reattempt to connect in ");
      Serial.print(_nextReconnectAttempt);
      Serial.println(" seconds");

      _nextReconnectAttempt += millis();
    }
  }
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  if ((char)payload[0] == '0')
  {
    _mqttSwitch->turnOff();
  }
  else if ((char)payload[0] == '1')
  {
    _mqttSwitch->turnOn();
  }
  else if ((char)payload[0] == '2')
  {
    _mqttSwitch->toggle();
  }
  // TODO Remove once SmartHome app is updated
  else if ((char)payload[0] == '3')
  {
    mqttSendStatus();
  }
}

void mqttSendStatus()
{

  if (_mqClient.connected())
  {
    SWITCH_STATE currentRelayState = (SWITCH_STATE)_mqttSwitch->state();

    sprintf(_jsonStatusBuffer, "{\"state\":\"%s\", \"status\":%i, \"chipId\":%i, \"ipAddress\":\"%s\", \"rssi\":\"%i dBm\"}",
            currentRelayState == SWITCH_ON ? "ON" : "OFF",
            (int)currentRelayState,
            ESP.getChipId(),
            WiFi.localIP().toString().c_str(),
            WiFi.RSSI());
    _mqClient.publish(_statusTopic, _jsonStatusBuffer, true);
  }
}
#endif