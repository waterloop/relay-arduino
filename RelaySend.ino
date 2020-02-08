// This Arduino will repeatedly send CAN packets! Can receive them as well.
// Used for testing the main Relay Arduino. 

#include <CAN.h>
#include <SD.h>
#include <SPI.h>

int BUFFER_SIZE_CAN = 8;
File packetLog;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed.");
  }

  if (!SD.begin(4)) {
    Serial.println("SD card init failed.");
  }

  // register callback for when CAN packet is received
  //CAN.onReceive(onReceive);
}

void loop() {
  
  // TODO: check signal from desktop and send to all microcontrollers

  // if packet is received from desktop, send can packet to controllers
  
  // save packet to sd card
  packetLog = SD.open("packetLog.txt", FILE_WRITE);
  if (packetLog) {
      packetLog.println("TCP packet: ");
      packetLog.close();
  } else {
    Serial.println("Error opening packetLog.txt");
  }

  char dataBuffer[] = "testing!";

  // create CAN packet
  Serial.println("Starting CAN packet");
  CAN.beginPacket(0x12);
  CAN.write(dataBuffer, BUFFER_SIZE_CAN);
  CAN.endPacket();
  Serial.println("Sent CAN Packet");
  delay(1000);
}

// if packet is received from controller, read data and send tcp to desktop
void onReceive() {
  Serial.println("Received CAN packet of size " + CAN.parsePacket());
  
  String data;
 
  while (CAN.available()) {
    char c = (char)CAN.read();
    data += c;
  }
  Serial.println("From CAN: " + data);
     
  // save packet to sd card
  packetLog = SD.open("packetLog.txt", FILE_WRITE);
  if (packetLog) {
    packetLog.println("CAN packet: ");
    packetLog.close();
  } else {
    Serial.println("Error opening packetLog.txt");
  }
}
