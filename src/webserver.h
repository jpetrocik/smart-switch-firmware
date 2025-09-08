#ifndef WEBSERVER_H
#define WEBSERVER_H

#ifdef WEBSERVER_ENABLED
void webServerSetup(DeviceConfig *deviceConfig);
void webServerLoop();
#endif
#endif