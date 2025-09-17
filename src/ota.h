#ifndef OTA_H
#define OTA_H

#ifdef OTA_ENABLED
void otaSetup(const char *hostname);
void otaLoop();
#endif
#endif