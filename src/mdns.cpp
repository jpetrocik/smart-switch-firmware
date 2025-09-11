#include "configuration.h"
#ifdef MDNS_ENABLED
#include <Arduino.h>
#include <ESP8266mDNS.h>

/******************************************
 * mDNS
 ******************************************/
void mdnsSetup(const char *hostname)
{
  Serial.println("Starting mDNS server....");
  MDNS.begin(hostname);
  MDNS.addService("socket", "tcp", 80);
}

void mdnsLoop()
{
  MDNS.update();
}
#endif