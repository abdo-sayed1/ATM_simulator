#ifndef WIFI_MODULE_H
#define WIFI_MODULE_H

#include <WiFi.h>

class WiFiModule {
public:
    static void begin(const char* ssid, const char* password);
    static bool isConnected();
};

#endif
