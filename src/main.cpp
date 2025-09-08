#include <FS.h>

#include <Ticker.h>
#include <ArduinoJson.h>
#include <Button2.h>
#include <ESP8266WiFi.h>

#include "configuration.h"
#include "relay.h"
#include "wifimanager.h"
#include "mqtt.h"
#include "mdns.h"
#include "ota.h"
#include "webserver.h"

Ticker ticker;

Button2 button;
Button2 relay;

DeviceConfig deviceConfig;

char apSsid[sizeof(CLIENT_ID) + 10];

void sendCurrentStatus(boolean changed);
void tick();
void configSave();
void configLoad();
void factoryReset();

void attach(float seconds)
{
  ticker.attach(seconds, tick);
}

void detach()
{
  ticker.detach();
  digitalWrite(LED_PIN, LED_OFF);
}

void buttonReleasedHandler(Button2 &btn)
{
  toogleRelay();
}

void longPressButtonHandler(Button2 &btn)
{
  if (btn.getLongClickCount() == 15)
  {
    digitalWrite(LED_PIN, LED_ON);
  }
  else if (btn.getLongClickCount() == 8)
  {
    deviceConfig.stopTicker();
    digitalWrite(LED_PIN, LED_OFF);
  }
  else if (btn.getLongClickCount() == 4)
  {
    deviceConfig.startTicker(0.25);
  }
}

void longReleaseButtonHandler(Button2 &btn)
{
  if (btn.getLongClickCount() >= 15)
  {
    factoryReset();
  }
  if (btn.getLongClickCount() >= 8)
  {
    ESP.reset();
  }
  else if (btn.getLongClickCount() >= 4)
  {
    wifi_manager_startAccessPointMode(apSsid);
  }
}

void handlerRelayStateChange(Button2 &btn)
{
  sendCurrentStatus(true);
  digitalWrite(LED_PIN, btn.isPressed() ? LED_OFF : LED_ON);
}

void wifiEventHandler(WIFI_MANAGER_EVENTS event)
{
  switch (event)
  {
  case STATION_STARTING:
    Serial.println("Connecting to WiFi");
    ticker.attach(.75, tick);
    break;
  case STATION_CONNECTED:
    Serial.println("Connected to WiFi");
    ticker.detach();
    digitalWrite(2, HIGH);
#ifdef MDNS_ENABLED
    mdnsSetup(deviceConfig.hostname);
#endif
    break;
  case STATION_DISCONNECTED:
    Serial.println("Disconnected from WiFi");
    ticker.attach(.75, tick);
    break;
  case ACCESS_POINT_STARTING:
    Serial.println("Starting AP");
    ticker.attach(.25, tick);
    break;
  }
}

void setup()
{
  Serial.begin(115200);

  Serial.println("GarageController Firmware");
  Serial.print("Build date: ");
  Serial.println(__DATE__ " " __TIME__);

  configLoad();

  // setup shared device
  deviceConfig.startTicker = attach;
  deviceConfig.stopTicker = detach;

  Serial.println(deviceConfig.hostname);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LED_OFF);

  pinMode(14, OUTPUT);
  digitalWrite(14, RELAY_OPEN);

  button.begin(BUTTON_PIN, INPUT);
  button.setClickHandler(buttonReleasedHandler);
  button.setLongClickTime(2000);
  button.setLongClickDetectedRetriggerable(true);
  button.setLongClickDetectedHandler(longPressButtonHandler);
  button.setLongClickHandler(longReleaseButtonHandler);

  relay.begin(RELAY_PIN);
  relay.setDebounceTime(500);
  relay.setChangedHandler(handlerRelayStateChange);

  sprintf(apSsid, CLIENT_ID, ESP.getChipId());
  wifi_manager_setEventHandler(wifiEventHandler);
  wifi_manager_setup(deviceConfig.wifiSsid, deviceConfig.wifiPassword, apSsid);

#ifdef OTA_ENABLED
  otaSetup(deviceConfig.hostname);
#endif

#ifdef MQTT_ENABLED
  mqttSetup(&deviceConfig);
#endif

#ifdef WEBSERVER_ENABLED
  webServerSetup(&deviceConfig);
#endif

  digitalWrite(LED_PIN, LED_OFF);
}

void loop()
{
  if (deviceConfig.dirty)
  {
    configSave();
    delay(2000);
    ESP.restart();
  }

  button.loop();
  relay.loop();

  wifi_manager_loop();

#ifdef MDNS_ENABLED
  mdnsLoop();
#endif

#ifdef MQTT_ENABLED
  mqttLoop();
#endif

#ifdef WEBSERVER_ENABLED
  webServerLoop();
#endif

#ifdef OTA_ENABLED
  otaLoop();
#endif
}

void sendCurrentStatus(boolean hasChanged)
{
#ifdef MQTT_ENABLED
  mqttSendStatus(hasChanged);
#endif
}

void factoryReset()
{
  Serial.println("Restoring Factory Setting....");
  WiFi.disconnect();
  SPIFFS.format();
  delay(500);
  Serial.println("Restarting....");
  ESP.restart();
}

void tick()
{
  int state = digitalRead(LED_PIN);
  digitalWrite(LED_PIN, !state);
}

// Called to save the configuration data after
// the device goes into AP mode for configuration
void configSave()
{
  JsonDocument jsonDoc;

  jsonDoc["device"] = deviceConfig.deviceName;
  jsonDoc["room"] = deviceConfig.roomName;
  jsonDoc["location"] = deviceConfig.locationName;
  jsonDoc["mqttHost"] = deviceConfig.mqttHost;
  jsonDoc["wifiSsid"] = deviceConfig.wifiSsid;
  jsonDoc["wifiPassword"] = deviceConfig.wifiPassword;

  sprintf(deviceConfig.hostname, "%s-%s", deviceConfig.roomName, deviceConfig.deviceName);

  File configFile = SPIFFS.open("/config.json", "w");
  if (configFile)
  {
    Serial.println("Saving config data....");
    serializeJson(jsonDoc, Serial);
    Serial.println();
    serializeJson(jsonDoc, configFile);
    configFile.close();
  }
}

// Loads the configuration data on start up
void configLoad()
{
  Serial.println("Loading config data....");
  if (SPIFFS.begin())
  {
    if (SPIFFS.exists("/config.json"))
    {
      // file exists, reading and loading
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile)
      {
        size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);

        JsonDocument jsonDoc;
        deserializeJson(jsonDoc, buf.get());

        serializeJsonPretty(jsonDoc, Serial);
        Serial.println();

        if (((JsonVariant)jsonDoc["device"]).is<const char *>())
        {
          strncpy(deviceConfig.deviceName, jsonDoc["device"], 20);
        }

        if (((JsonVariant)jsonDoc["room"]).is<const char *>())
        {
          strncpy(deviceConfig.roomName, jsonDoc["room"], 20);
        }

        if (((JsonVariant)jsonDoc["location"]).is<const char *>())
        {
          strncpy(deviceConfig.locationName, jsonDoc["location"], 20);
        }

        sprintf(deviceConfig.hostname, "%s-%s", deviceConfig.roomName, deviceConfig.deviceName);

        if (((JsonVariant)jsonDoc["mqttHost"]).is<const char *>())
        {
          strncpy(deviceConfig.mqttHost, jsonDoc["mqttHost"], 50);
        }
        else
        {
          deviceConfig.mqttHost[0] = 0;
        }

        if (((JsonVariant)jsonDoc["wifiSsid"]).is<const char *>())
        {
          strncpy(deviceConfig.wifiSsid, jsonDoc["wifiSsid"], 25);
        }
        else
        {
          deviceConfig.wifiSsid[0] = 0;
        }

        if (((JsonVariant)jsonDoc["wifiPassword"]).is<const char *>())
        {
          strncpy(deviceConfig.wifiPassword, jsonDoc["wifiPassword"], 25);
        }
        else
        {
          deviceConfig.wifiPassword[0] = 0;
        }
      }
    }
  }
}
