#include "configuration.h"

#ifdef MQTT_ENABLED
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include "mqtt.h"
#include "device.h"

WiFiClient _espClient;
PubSubClient _mqClient(_espClient);

int _reconnectAttemptCounter = 0;
unsigned long _nextReconnectAttempt = 0;

char _commandTopic[100];
char _statusTopic[100];
char _lwtTopic[100];
char _jsonStatusBuffer[140];
unsigned long lastStatePublishCounter = 0;

void mqttCallback(char *topic, byte *payload, unsigned int length);
void mqttConnect();

void mqttSetup()
{
  DeviceConfig* deviceConfig = currentDeviceConfig();

  if (strlen(deviceConfig->mqttHost) == 0)
  {
    Serial.println("MQTT host not configured");
    return;
  }

  Serial.println("Connecting to MQTT Server....");
  _mqClient.setServer(deviceConfig->mqttHost, 1883);
  _mqClient.setCallback(mqttCallback);
  //  _mqClient.setKeepAlive(120);

  sprintf(_commandTopic, "%s/%s/%s/command", deviceConfig->locationName, deviceConfig->roomName, deviceConfig->deviceName);
  sprintf(_statusTopic, "%s/%s/%s/status", deviceConfig->locationName, deviceConfig->roomName, deviceConfig->deviceName);
  sprintf(_lwtTopic, "%s/%s/%s/LWT", deviceConfig->locationName, deviceConfig->roomName, deviceConfig->deviceName);
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

    DeviceConfig* deviceConfig = currentDeviceConfig();

  if (strlen(deviceConfig->mqttHost) == 0)
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
  DeviceState* currentState = currentDeviceState();
  if ((char)payload[0] == '0')
  {
    currentState->relayState = RELAY_OPEN;
  }
  else if ((char)payload[0] == '1')
  {
    currentState->relayState = RELAY_CLOSED;
  }
  else if ((char)payload[0] == '2')
  {
    currentState->relayState = currentState->relayState == RELAY_OPEN ? RELAY_CLOSED : RELAY_OPEN;
  }
}

void mqttSendStatus()
{

  if (_mqClient.connected())
  {
    _mqClient.publish(_statusTopic, currentDeviceStateJson(), true);
  }
}
#endif