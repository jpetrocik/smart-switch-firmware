#ifndef WEBSERVER_H
#define WEBSERVER_H

#ifdef WEBSERVER_ENABLED
void webServerSetup(DeviceConfig *deviceConfig, Switch *switch1);
void webServerLoop();
#endif
#endif