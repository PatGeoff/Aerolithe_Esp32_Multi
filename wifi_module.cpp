// wifi_module.cpp
#include "wifi_module.h"

void connectToWiFi(const char* ssid, const char* password, IPAddress local_IP, IPAddress gateway, IPAddress subnet) {
  WiFi.config(local_IP, gateway, subnet);  // rendre l'IP statique
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Pas encore cconnecté à Aérolithen\n");
  }
  Serial.print("Maintenant connecté à Aérolithe\n");
}

bool isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}


void checkAndReconnectWiFi(const char* ssid, const char* password, IPAddress local_IP, IPAddress gateway, IPAddress subnet) {
  unsigned long currentMillis = millis();
  static unsigned long lastAttemptTime = 0;
  if (currentMillis - lastAttemptTime > 120000) {  // check every 2 minutes
    lastAttemptTime = currentMillis;
    if (WiFi.status() != WL_CONNECTED) {
      connectToWiFi(ssid, password, local_IP, gateway, subnet);
      Serial.print("Pas connecté à Aérolithe\n");
    } else {
      Serial.print("Bien connecté à Aérolithe\n");
    }
  }
}
