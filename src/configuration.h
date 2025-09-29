#ifndef Configuration_h
#define Configuration_h

#include <Arduino.h>
#include "relay.h"

#define CLIENT_ID "switch-%i"

// #define SONOFF_BASIC
// #define KMC_SMART_PLUG
// #define SONOFF_DUAL_R2
// #define KS602S_SWITCH
#define SANA_SW02_03_v1
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

/**
 * SW02-03 has two versions, the v1 with a esp8266 base Tuya
 * chip and the v2 with a non-esp base chip. v1 just needs to be flashed, 
 * v2 needs the Tuya chip removed and replaced with an esp8266 chip. 
 * 
 * The v1 version does not have pins for led controls.  The leds are tied
 * to the relay circuit and turns on/off based on the relay state.
 * 
 * The v2 version has pins for led control. The wifi pin is tied to ADC pin
 * and hence can't be used for output to contorl the LED.  See hack here to 
 * control the WiFi led, https://github.com/r2db/Youngzuth_SW02-03
 */
#ifdef SANA_SW02_03_v1
#define HIGH_BUTTON_PIN 12
#define HIGH_RELY_PIN 13
#define LOW_BUTTON_PIN 5
#define LOW_RELY_PIN 15
#define OFF_BUTTON_PIN 3
#define LED_PIN 0
#define LED_INVERTED
#endif

#ifdef SANA_SW02_03_v2
#define HIGH_BUTTON_PIN 12
#define HIGH_RELY_PIN 13
#define HIGH_LED 0
#define LOW_BUTTON_PIN 5
#define LOW_RELY_PIN 15
#define LOW_LED 14
#define OFF_BUTTON_PIN 3
#define LED_PIN 16
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
