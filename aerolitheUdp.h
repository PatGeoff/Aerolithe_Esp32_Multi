// aerolitheUdp.h

#ifndef UDP_H
#define UDP_H

#include <WiFiUDP.h>
#include <WiFi.h>
#include "stepper.h"
#include "wifi_module.h"

extern const int udpPort;


void startAerolitheUDP();
void udpGetIncoming();
void actuatorCommand(const char* packetBuffer);
void stepMotorCommand(const char* packetBuffer);
void liftCommand(const char* packetBuffer);
void sendResponse(const char* message);

#endif
