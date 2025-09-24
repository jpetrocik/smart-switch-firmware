#ifndef Configuration_h
#define Configuration_h

#include <Arduino.h>
#include "relay.h"

#define CLIENT_ID "switch-%i"

// #define SONOFF_BASIC
// #define KMC_SMART_PLUG
// #define SONOFF_DUAL_R2
// #define KS602S_SWITCH
#define SANA_SWITCH_3GANG
// #define ESP8266_DEV

#ifdef SONOFF_BASIC
#define RELAY_PIN 12
#define LED_PIN 13
#define BUTTON_PIN 0
#define LED_INVERTED
#endif

#ifdef KMC_SMART_PLUG
#define RELAY_PIN 14
#define LED_PIN 13
#define BUTTON_PIN 0
#define LED_INVERTED
#endif

#ifdef SONOFF_DUAL_R2
#define RELAY_PIN 14
#define LED_PIN 13
#define BUTTON_PIN 0
#define LED_INVERTED
#define RELAY_INVERTED
#endif

/**
 * 1MB Flash
 */
#ifdef KS602S_SWITCH
#define RELAY_PIN 12
#define LED_PIN 13
#define BUTTON_PIN 0
#endif

#ifdef SANA_SWITCH_3GANG
#define HIGH_BUTTON_PIN 12
#define HIGH_RELY_PIN 6
#define LOW_BUTTON_PIN 5
#define LOW_RELY_PIN 7
#define OFF_BUTTON_PIN 3
#define LED_PIN 2
#endif

#ifdef ESP8266_DEV
#define RELAY_PIN 12
#define LED_PIN 2
#define BUTTON_PIN 0
#define LED_INVERTED
#endif

#ifdef LED_INVERTED
#define LED_ON 0
#define LED_OFF 1
#else
#define LED_ON 1
#define LED_OFF 0
#endif

#define MQTT_ENABLED
#define WEBSERVER_ENABLED
#define OTA_ENABLED
#define MDNS_ENABLED

struct DeviceConfig
{
  char deviceName[20] = "fan";
  char roomName[20] = "hallway";
  char locationName[20] = "house";
  char hostname[41] = "hallway-fan";
  char mqttHost[50] = "hermes.petrocik.net";
  char wifiSsid[25]= "PNET";
  char wifiPassword[50] = "5626278472";
  bool disableLed = false;
  bool dirty = false;
};

#endif
