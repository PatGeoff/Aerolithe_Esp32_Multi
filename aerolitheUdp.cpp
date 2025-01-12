// aerolitheUdp.cpp

#include "aerolitheUdp.h"
#include "actuator.h"
#include "stepper.h"
#include "lift.h"

WiFiUDP udp;
IPAddress senderIP = IPAddress();  // Address of remote computer sending messages, initializes to 0.0.0.0
unsigned int senderPort = 55544;   // Port of the remote coputer sending messages, initializes to 0;
//unsigned int senderPort = 12345;   // For testing with UDP Test Tools 3.0
// DEBUGGING
const IPAddress windows_IP(192, 168, 2, 7);  // À cause de Parallels Desktop, c'est l'adresse de Windows en mode Bridged Wifi et elle peut changer alors vérifier

int rotarySpeed = 0;

void startAerolitheUDP() {
  udp.begin(udpPort);
  Serial.println("Maintenant à l'écoute sur le port 55544");
  sendResponse("Maintenant à l'écoute sur le port 55544");
}

void udpGetIncoming() {
  int packetSize = udp.parsePacket();

  if (packetSize) {

    // Read the packet into packetBuffer
    char packetBuffer[255];  // Buffer to hold incoming packet
    int len = udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;  // Null-terminate the string
    }
    //senderIP = windows_IP;
    //senderIP = udp.remoteIP();
    //senderPort = udp.remotePort();
    Serial.print("Received UDP packet: ");
    Serial.print(packetBuffer);
    String stri = "   --> The senderIP is " + senderIP.toString() + " and the senderPort is " + String(senderPort);
    Serial.println(stri);

    // Copy the packetBuffer for command parsing
    char commandBuffer[255];
    strncpy(commandBuffer, packetBuffer, sizeof(commandBuffer));
    commandBuffer[sizeof(commandBuffer) - 1] = 0;  // Ensure null termination


    char* token = strtok(commandBuffer, " ");
    if (token != NULL) {
      String command = String(token);
      if (command.equals("status")) {
        //sendResponse("Connexion au ESP32 de la boîte blanche bien établie");
        sendResponse("ok esp32");
      } else if (command.equals("fullSetup")) {
        setup();
      } else if (command.equals("actuator")) {
        actuatorCommand(packetBuffer);
      } else if (command.equals("stepmotor")) {
        stepMotorCommand(packetBuffer);
      } else if (command.equals("lift")) {
        liftCommand(packetBuffer);
      } else {
        Serial.println("Error: Unknown command.");
      }

    } else {
      Serial.println("Error: Failed to parse command.");
    }
  }
}


void actuatorCommand(const char* packetBuffer) {
  Serial.print("Parsing packetBuffer: ");
  Serial.println(packetBuffer);
  // int command;
  // int parsed = sscanf(packetBuffer, "actuator,%d", &command);
  if (strcmp(packetBuffer, "actuator zero") == 0) {
    actuatorDown(5000);
    delay(1000);
    actuatorSetZeroPosition();
    return;
  }
  if (strcmp(packetBuffer, "actuator 5") == 0) {
    actuatorGoTo5degres();
    return;
  }
  if (strcmp(packetBuffer, "actuator 25") == 0) {
    actuatorGoTo25degres();
    return;
  }
  if (strcmp(packetBuffer, "actuator 45") == 0) {
    actuatorGoTo45degres();
    return;
  }
}

void liftCommand(const char* packetBuffer) {
  // Debug prints to help diagnose the problem
  Serial.print("Parsing packetBuffer: ");
  Serial.println(packetBuffer);

  if (strcmp(packetBuffer, "lift setZero") == 0) {
    Serial.println("lift Go to zero");
    liftServoSetZero();
    return;
  }
  if (strncmp(packetBuffer, "lift position", 13) == 0) {
    int position;
    int parsed = sscanf(packetBuffer, "lift position %i", &position);
    if (parsed == 1) {
      liftPerformSetPosition(position);
    } else {
      Serial.println("Failed to parse position");
    }
    return;
  }
  if (strcmp(packetBuffer, "lift getPosition") == 0) {
    getLiftPosition();
    return;
  }

  return;
}

void stepMotorCommand(const char* packetBuffer) {

  // Debug prints to help diagnose the problem
  Serial.print("Parsing packetBuffer: ");
  Serial.println(packetBuffer);
  /*
  // NearCalibration will bring the camera to trigger the nearLimitSwitch and reset the stepper motor's position to zero
  if (strcmp(packetBuffer, "stepmotor nearcalibration") == 0) {
    // Call the calibration method
    //performStepperMotorNearLimitCalibration();

    return;
  }*/
  if (strcmp(packetBuffer, "stepmotor calibration") == 0) {
    // Call the calibration method
    stepperReadSwitches();
    performStepperMotorFarLimitCalibration();
    return;
  }
  if (strcmp(packetBuffer, "stepmotor stop") == 0) {
    // Call the stop method
    stepperMotorEmergencyStop = true;
    return;
  }

  if (strcmp(packetBuffer, "stepmotor setZero") == 0) {
    String message = "stepperCurrentPosition was set to 0";
    Serial.println(message);
    setStepperZeroRef();
    return;
  }
  if (strcmp(packetBuffer, "stepmotor getstepperposition") == 0) {
    String message = "stepperCurrentPosition, " + String(stepper.currentPosition());
    sendResponse(message.c_str());
    return;
  }

    int speed;
  long position;
  int parsed = sscanf(packetBuffer, "stepmotor moveto %d,%i,0", &speed, &position);
  Serial.println("Message Received:  speed = " + (String)speed + " and position = " + (String)position + " and parsed items = " + (String)parsed + "\n");
  Serial.println("actual position: " + (String)stepper.currentPosition());
  try {
    //performStepperMotorMoveTo(speed, position / microsteps);
    performStepperMotorMoveTo(speed, position);
  } catch (...) {
    Serial.println("Error: Invalid stepmotor command format.");
  }
}



void sendResponse(const char* message) {
  Serial.print("Sending response: ");
  Serial.print(message);
  Serial.print(" on port ");
  Serial.println(senderPort);

  udp.beginPacket(windows_IP, senderPort);
  udp.write((const uint8_t*)message, strlen(message));  // Correct conversion and length

  int result = udp.endPacket();
  if (result == 1) {
    Serial.println("Packet sent successfully");
  } else {
    Serial.print("Error sending packet, result: ");
    Serial.println(result);
  }
}