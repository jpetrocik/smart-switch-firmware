#ifndef MDNS_H
#define MDNS_H

#ifdef MDNS_ENABLED
void mdnsSetup(const char *hostname);
void mdnsLoop();
#endif
#endif