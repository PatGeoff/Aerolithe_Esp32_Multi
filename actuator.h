// actuator.h

#ifndef ACTUATOR_H
#define ACTUATOR_H

// Include Arduino library for pin definitions
#include <Arduino.h>

// Define output pins for the relay
#define RELAY_PIN_UP 25    // GPIO pin connecté à l'actuateur
#define RELAY_PIN_DOWN 26  // GPIO pin connecté à l'actuateur
#define HALL1_PIN 32       // GPIO pin connecté à Hall1
#define HALL2_PIN 33       // GPIO pin connecté à Hall2



// Function prototypes
void initializeActuator();                  // Initialisation des pins
void actuatorUp(unsigned long duration);    // faire monter la caméra
void actuatorDown(unsigned long duration);  // faire descendre la caméra
void readHalls();                           // lecture des sensors de position
void actuatorPosition();                    // affiche la position
void setOffset();                           // définir un offset par rapport au zéro
float calculatePosition();                  // calcule de la position
void updatePosition();
void actuatorSetZeroPosition();
void sensor1Interrupt();
void sensor2Interrupt();
void actuatorGoTo5degres();
void actuatorGoTo25degres();
void actuatorGoTo45degres();
void actuatorGoToPosition();
extern bool actuatorSetZeroDone;


#endif  // ACTUATOR_H
