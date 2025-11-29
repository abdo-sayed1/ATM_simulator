#include "WiFiModule.h"

void WiFiModule::begin(const char* ssid, const char* password) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    unsigned long startAttempt = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 8000) {
        delay(200);
    }
}

bool WiFiModule::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}
