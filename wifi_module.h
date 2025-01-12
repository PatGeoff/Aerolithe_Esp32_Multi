// wifi_module.h
#ifndef WIFI_MODULE_H
#define WIFI_MODULE_H

#include <WiFi.h>

void connectToWiFi(const char* ssid, const char* password, IPAddress local_IP, IPAddress gateway,IPAddress subnet);
void checkAndReconnectWiFi(const char* ssid, const char* password, IPAddress local_IP, IPAddress gateway,IPAddress subnet);
bool isWiFiConnected();

#endif // WIFI_MODULE_H