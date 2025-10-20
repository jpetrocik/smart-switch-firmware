#include "configuration.h"

#ifdef WEBSERVER_ENABLED
#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "device.h"

ESP8266WebServer server(80);

void webServerLoop()
{
  server.handleClient();
}

void handleTurnOff()
{
  switchTurnOff();
  server.send(200);
}

void handleTurnOn()
{
  switchTurnOn();
  server.send(200);
}

void handleStatus()
{
  server.send(200, "application/json", currentDeviceStateJson());
}

void handleRestart()
{
  server.send(200, "application/json", "{\"message\":\"Restarting\"}");
  delay(1000);
  deviceRestart();
}

void handleConfigureDevice()
{
  server.send(200, "application/json", currentDeviceConfigJson());
}

void handleSaveConfigureDevice()
{
  DeviceConfig *deviceConfig = currentDeviceConfig();
  int argCount = server.args();
  for (int i = 0; i < argCount; i++)
  {
    String argName = server.argName(i);
    String argValue = server.arg(i);

    if (argName == "device")
    {
      argValue.toCharArray(deviceConfig->deviceName, 20);
    }
    else if (argName == "room")
    {
      argValue.toCharArray(deviceConfig->roomName, 20);
    }
    else if (argName == "location")
    {
      argValue.toCharArray(deviceConfig->locationName, 20);
    }
    else if (argName == "mqttHost")
    {
      argValue.toCharArray(deviceConfig->mqttHost, 50);
    }
    else if (argName == "ssid")
    {
      argValue.toCharArray(deviceConfig->wifiSsid, 50);
    }
    else if (argName == "password")
    {
      argValue.toCharArray(deviceConfig->wifiPassword, 50);
    }
    else if (argName == "disableLed")
    {
      deviceConfig->disableLed = argValue == "true";
    }
  }

  deviceConfig->dirty = true;

  server.send(200);
}

void webServerSetup()
{
  Serial.println("Starting web server on port 80");
  server.on("/", handleStatus);
  server.on("/switch/on", handleTurnOn);
  server.on("/switch/off", handleTurnOff);
  server.on("/restart", HTTP_POST, handleRestart);
  server.on("/config", HTTP_GET, handleConfigureDevice);
  server.on("/config", HTTP_PUT, handleSaveConfigureDevice);

  server.begin();
}
#endif