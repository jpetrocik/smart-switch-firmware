#include "configuration.h"

#ifdef WEBSERVER_ENABLED
#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "switch.h"

ESP8266WebServer server(80);
char webserver_jsonStatusBuffer[300];
DeviceConfig *webserver_deviceConfig;
Switch *webserver_switch;

void webServerLoop()
{
  server.handleClient();
}

void handleTurnOff()
{
  webserver_switch->turnOff();
  server.send(200);
}

void handleTurnOn()
{
  webserver_switch->turnOn();
  server.send(200);
}

void handleStatus()
{
  sprintf(webserver_jsonStatusBuffer, "{\"status\":%i, \"changed\":false}", (int)webserver_switch->state());
  server.send(200, "application/json", webserver_jsonStatusBuffer);
}

void handleRestart()
{
  server.send(200, "application/json", "{\"message\":\"Restarting\"}");
  delay(1000);
  ESP.restart();
}

// TODO SHould return deviceConfig
void handleConfigureDevice()
{
  Serial.println("Loading config data....");
  if (SPIFFS.exists("/config.json"))
  {
    // file exists, reading and loading
    File configFile = SPIFFS.open("/config.json", "r");
    if (configFile)
    {
      size_t size = configFile.size();
      std::unique_ptr<char[]> buf(new char[size + 1]);

      configFile.readBytes(buf.get(), size);
      buf.get()[size] = 0;

      server.send(200, "application/json", buf.get());
      return;
    }
  }
  server.send(200, "application/json", "{}");
}

void handleSaveConfigureDevice()
{
  int argCount = server.args();
  for (int i = 0; i < argCount; i++)
  {
    String argName = server.argName(i);
    String argValue = server.arg(i);

    if (argName == "device")
    {
      argValue.toCharArray(webserver_deviceConfig->deviceName, 20);
    }
    else if (argName == "room")
    {
      argValue.toCharArray(webserver_deviceConfig->roomName, 20);
    }
    else if (argName == "location")
    {
      argValue.toCharArray(webserver_deviceConfig->locationName, 20);
    }
    else if (argName == "mqttHost")
    {
      argValue.toCharArray(webserver_deviceConfig->mqttHost, 50);
    }
    else if (argName == "ssid")
    {
      argValue.toCharArray(webserver_deviceConfig->wifiSsid, 50);
    }
    else if (argName == "password")
    {
      argValue.toCharArray(webserver_deviceConfig->wifiPassword, 50);
    }
    else if (argName == "disableLed")
    {
      webserver_deviceConfig->disableLed = argValue == "true";
    }
  }

  webserver_deviceConfig->dirty = true;

  server.send(200);
}

void webServerSetup(DeviceConfig *deviceConfig, Switch *switch1)
{
  webserver_deviceConfig = deviceConfig;
  webserver_switch = switch1;

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