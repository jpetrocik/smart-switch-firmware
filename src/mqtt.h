#ifndef MQTT_H
#define MQTT_H

#ifdef MQTT_ENABLED
#include <Arduino.h>

void mqttSetup(DeviceConfig *deviceConfig);
void mqttLoop();
void mqttSendStatus();
#endif
#endif