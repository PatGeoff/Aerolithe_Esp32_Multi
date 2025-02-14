//Aerolithe_StepMotorAndActuator.ino

// Code pour aller avec un ESP32-Wroom DevKit1 Wifi+BlueTooth
// https://abra-electronics.com/robotics-embedded-electronics/esp-series/wifi-esp32-esp-wroom-32-wi-fi-bluetooth-ble-low-power-iot-microcontroller.html
// utiliser le board ESP32 Dev Module ici dans Arduino IDE


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

unsigned long previousMillisTest;
const long intervalTest = 1000;

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
  udpGetIncoming();      
  debounceLimitSwitches();  // Debounces the stepper motor limit switches to get clean readings   // Check incomping UDP messages
  if (!runSpeedBool){
     stepper.run();            // Allow the stepper motor to run at a fixed speed to a position
  }
  else{                         
    stepper.runSpeed();          // Allow the stepper motor to run loose at a defined speed
  }
  if (farLimitInterruptTriggered || nearLimitInterruptTriggered){
    stepper.setSpeed(0);
    stepper.runSpeed();
    //Serial.println("Stepper -> Limit Triggered");
  }

  // // Non-blocking print of enablePin state every second
  // unsigned long currentMillis = millis();
  // if (currentMillis - previousMillisTest >= intervalTest) {
  //   previousMillisTest = currentMillis;
  //   Serial.print("enablePin state: ");
  //   Serial.println(digitalRead(enablePin));
  // }

 farLimitInterruptTriggered = nearLimitInterruptTriggered = false;
  
  //readHalls();              // Reads the linear actuator Halls  
  
}
