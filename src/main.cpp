#include <FS.h>
#include <Ticker.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#include "configuration.h"
#include "relay.h"
#include "wifimanager.h"
#include "mqtt.h"
#include "mdns.h"
#include "ota.h"
#include "webserver.h"
#include "switch.h"
#include "device.h"

Ticker ticker;

Switch mainSwitch;

char apSsid[sizeof(CLIENT_ID) + 10];

void sendCurrentStatus();
void tick();
void configSave();
void configLoad();
void factoryReset();

void startTicker(float seconds)
{
  ticker.attach(seconds, tick);
}

void stopTicker()
{
  ticker.detach();
  digitalWrite(LED_PIN, LED_OFF);
}

/**
 * Called every 2s while the button is being pressed
 * to indicate via the LED what the current action
 * will be when the button is released
 *
 * 8s-16s LED flashes quickly. Switch to AP mode, used to reset wifi credentials
 * 16s-30s LED turns off. Reboot the device
 * 30s- LED turns on. Factory Reset
 */
void longPressButtonHandler(Button2 &btn)
{
  if (btn.getLongClickCount() == 15)
  {
    digitalWrite(LED_PIN, LED_ON);
  }
  else if (btn.getLongClickCount() == 8)
  {
    stopTicker();
    digitalWrite(LED_PIN, LED_OFF);
  }
  else if (btn.getLongClickCount() == 4)
  {
    startTicker(0.25);
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
    deviceRestart();
  }
  else if (btn.getLongClickCount() >= 4)
  {
    wifi_manager_startAccessPointMode(apSsid);
  }
}

void handleSwitchStateChange(SWITCH_STATE state)
{
  sendCurrentStatus();
  if (!currentDeviceConfig()->disableLed)
  {
    digitalWrite(LED_PIN, state == SWITCH_ON ? LED_ON : LED_OFF);
  }
}

void wifiEventHandler(WIFI_MANAGER_EVENTS event)
{
  switch (event)
  {
  case STATION_STARTING:
    Serial.println("Connecting to WiFi");
    startTicker(.75);
    break;
  case STATION_CONNECTED:
    Serial.println("Connected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    stopTicker();
    digitalWrite(LED_PIN, LED_OFF);
#ifdef MDNS_ENABLED
    mdnsSetup(currentDeviceConfig()->hostname);
#endif
    break;
  case STATION_DISCONNECTED:
    Serial.println("Disconnected from WiFi");
    startTicker(.75);
    break;
  case ACCESS_POINT_STARTING:
    Serial.println("Starting AP");
    startTicker(.25);
    break;
  case STATION_AUTH_FAILED:
    Serial.println("Authentication Failed");
    startTicker(.25);
    break;
  }
}

void setup()
{
  Serial.begin(115200);

  Serial.println("Switch Firmware");
  Serial.print("Build date: ");
  Serial.println(__DATE__ " " __TIME__);

  configLoad();
  DeviceConfig *deviceConfig = currentDeviceConfig();

  Serial.println(deviceConfig->hostname);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LED_OFF);

  mainSwitch.setup(BUTTON_PIN, RELAY_PIN);
  mainSwitch.setStateChangedHandler(handleSwitchStateChange);
  mainSwitch.setupLongClickHandler(longPressButtonHandler, longReleaseButtonHandler);

  snprintf(apSsid, sizeof(apSsid), CLIENT_ID, ESP.getChipId());

  wifi_manager_setEventHandler(wifiEventHandler);
  wifi_manager_setup(deviceConfig->wifiSsid, deviceConfig->wifiPassword, apSsid);

#ifdef OTA_ENABLED
  otaSetup(deviceConfig->hostname);
#endif

#ifdef MQTT_ENABLED
  mqttSetup();
#endif

#ifdef WEBSERVER_ENABLED
  webServerSetup();
#endif
}

void loop()
{
  DeviceConfig *deviceConfig = currentDeviceConfig();

  if (deviceConfig->dirty)
  {
    configSave();
    delay(2000);
    ESP.restart();
  }

  // //TODO Can be directed change by the switch class when the button is pressed
  // // but the state is not updated. Maybe a currentState and requestedState. Also
  // // using the switch as the current state rather then maintaining a separate state
  // // in the device state.  Maybe make device state dynamic. in 
  // // Sync up the relay state in case it was changed
  // if (currentDeviceState()->switchState == SWITCH_ON)
  // {
  //   mainSwitch.turnOn();
  // }
  // else
  // {
  //   mainSwitch.turnOff();
  // }

  mainSwitch.loop();

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

void sendCurrentStatus()
{
#ifdef MQTT_ENABLED
  mqttSendStatus();
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

void configSave()
{

  File configFile = SPIFFS.open("/config.json", "w");
  if (configFile)
  {
    Serial.println("Saving config data....");
    configFile.write(currentDeviceConfigJson());
    configFile.close();
  }
}

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

        DeviceConfig *deviceConfig = currentDeviceConfig();
        if (((JsonVariant)jsonDoc["device"]).is<const char *>())
        {
          strncpy(deviceConfig->deviceName, jsonDoc["device"], 20);
        }

        if (((JsonVariant)jsonDoc["room"]).is<const char *>())
        {
          strncpy(deviceConfig->roomName, jsonDoc["room"], 20);
        }

        if (((JsonVariant)jsonDoc["location"]).is<const char *>())
        {
          strncpy(deviceConfig->locationName, jsonDoc["location"], 20);
        }

        if (((JsonVariant)jsonDoc["mqttHost"]).is<const char *>())
        {
          strncpy(deviceConfig->mqttHost, jsonDoc["mqttHost"], 50);
        }
        else
        {
          deviceConfig->mqttHost[0] = 0;
        }

        if (((JsonVariant)jsonDoc["wifiSsid"]).is<const char *>())
        {
          strncpy(deviceConfig->wifiSsid, jsonDoc["wifiSsid"], 25);
        }
        else
        {
          deviceConfig->wifiSsid[0] = 0;
        }

        if (((JsonVariant)jsonDoc["wifiPassword"]).is<const char *>())
        {
          strncpy(deviceConfig->wifiPassword, jsonDoc["wifiPassword"], 25);
        }
        else
        {
          deviceConfig->wifiPassword[0] = 0;
        }

        if (((JsonVariant)jsonDoc["disableLed"]).is<bool>())
        {
          deviceConfig->disableLed = jsonDoc["disableLed"];
        }
        else
        {
          deviceConfig->disableLed = false;
        }

        size_t l = snprintf(deviceConfig->hostname, sizeof(deviceConfig->hostname), "%s-%s", deviceConfig->roomName, deviceConfig->deviceName);
        if (l >= sizeof(deviceConfig->hostname))
        {
          deviceConfig->hostname[sizeof(deviceConfig->hostname) - 1] = '\0';
        }

        configFile.close();
      }
    }
  }
}

void switchTurnOn()
{
  mainSwitch.turnOn();
}

void switchTurnOff()
{
  mainSwitch.turnOff();
}

void switchToggle()
{
  mainSwitch.toggle();
}

SWITCH_STATE switchState()
{
  return mainSwitch.state();
}

void deviceRestart()
{
  Serial.println("Restarting device...");
  ESP.restart();
}
