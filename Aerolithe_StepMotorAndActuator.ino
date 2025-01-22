#include <WiFi.h>
#include "wifi_module.h"
#include "stepper.h"
#include "aerolitheUdp.h"
#include "actuator.h"
#include "lift.h"
//#include "ledStrip.h"




const char *ssid = "Aerolithe";
const char *password = "Galactium2013";
const IPAddress local_IP(192, 168, 2, 11);  // THIS IP ADDRESS
const IPAddress gateway(192, 168, 2, 1);
const IPAddress subnet(255, 255, 255, 0);
const int udpPort = 44455;  // THIS RECEIVING PORT




// UTILISER le BOARD ESP32-WROOM-DA Module

void setup() {
  Serial.begin(115200);
  connectToWiFi(ssid, password, local_IP, gateway, subnet);
  startAerolitheUDP();  // Starts UDP protocol
  initializeActuator();
  initializeStepperLimitSwitches();  //
  actuatorDown(2000);         // Sends the actuator down
  actuatorSetZeroPosition();          // Sets the actuator's zero position
  //liftServoSetup();
  int increment = 0;
  // Give time to the actuator to go down before sending message.
  while (!actuatorSetZeroDone || increment < 5000){
    increment += 1;
  }
  liftServoSetZero();
  sendResponse("ESP32 Wroom - full setup done");
  Serial.println("ESP32 Wroom - full setup done");

}

void loop() {
  checkAndReconnectWiFi(ssid, password, local_IP, gateway, subnet);
  udpGetIncoming();         // Check incomping UDP messages
  if (!runSpeedBool){
     stepper.run();            // Allow the stepper motor to run at a fixed speed to a position
  }
  else{                         
    stepper.runSpeed();          // Allow the stepper motor to run loose at a defined speed
  }
 
  debounceLimitSwitches();  // Debounces the stepper motor limit switches to get clean readings
  readHalls();              // Reads the linear actuator Halls  
  
}
