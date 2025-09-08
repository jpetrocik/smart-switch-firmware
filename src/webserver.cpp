#include "configuration.h"

#ifdef WEBSERVER_ENABLED
#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "relay.h"

ESP8266WebServer server(80);
char webserver_jsonStatusBuffer[300];
DeviceConfig *webserver_deviceConfig;

void webServerLoop()
{
  server.handleClient();
}

void handleOpenRelay()
{
  openRelay();
  server.send(200);
}

void handleCloseRelay()
{
  closeRelay();
  server.send(200);
}

void handleStatus()
{
  sprintf(webserver_jsonStatusBuffer, "{\"status\":%i, \"changed\":false}", (int)relayState());
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
      std::unique_ptr<char[]> buf(new char[size]);

      configFile.readBytes(buf.get(), size);

      JsonDocument json;
      DeserializationError error = deserializeJson(json, buf.get());

      if (!error)
      {
        String result;
        serializeJsonPretty(json, webserver_jsonStatusBuffer);
        server.send(200, "application/json", result);
        return;
      }
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
  }

  webserver_deviceConfig->dirty = true;

  server.send(200);
}

void webServerSetup(DeviceConfig *deviceConfig)
{
  webserver_deviceConfig = deviceConfig;

  Serial.println("Starting web server on port 80");
  server.on("/", handleStatus);
  server.on("/open", handleCloseRelay);
  server.on("/close", handleOpenRelay);
  server.on("/restart", HTTP_POST, handleRestart);
  server.on("/config", HTTP_GET, handleConfigureDevice);
  server.on("/config", HTTP_PUT, handleSaveConfigureDevice);

  server.begin();
}
#endif