#ifndef Configuration_h
#define Configuration_h

#include <Arduino.h>

#define CLIENT_ID "switch-%i"

// #define SONOFF_BASIC
// #define KMC_SMART_PLUG
// #define SONOFF_DUAL_R2
#define KS602S_SWITCH

#ifdef SONOFF_BASIC
#define RELAY_PIN 12
#define LED_PIN 13
#define LED_INVERTED
#define RELAY_INVERTED
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
  char deviceName[20] = "name";
  char roomName[20] = "room";
  char locationName[20] = "house";
  char hostname[41] = "room-name";
  char mqttHost[50];
  char wifiSsid[25];
  char wifiPassword[50];
  boolean dirty = false;
  void (*startTicker)(float seconds);
  void (*stopTicker)();
};

#endif
