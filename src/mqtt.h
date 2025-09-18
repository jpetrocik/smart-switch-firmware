#ifndef MQTT_H
#define MQTT_H

#ifdef MQTT_ENABLED
#include <Arduino.h>
#include "switch.h"

void mqttSetup(DeviceConfig *deviceConfig, Switch *switch1);
void mqttLoop();
void mqttSendStatus();
#endif
#endif