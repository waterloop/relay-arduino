// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <CAN.h>
#include <SPI.h>
#include <SD.h>
#include <ArduinoModbus.h>
#include <cassert>

File packetStore;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // CAN bus init at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1); // This endlessly loops at this point so no further code is executed
  }

  // SD card init
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  packetStore = SD.open("test.txt", FILE_WRITE);

  // Creating modbus TCP client for Arduino to connect to desktop
  ModbusTCPClient client;
  // Connecting client to server

  auto start_time = millis();
  auto end_time = start_time;

  // While the client has not successfully connected to the server,
  // keep retrying connection. After a ten seconds, stop trying.
  while (!client.begin("192.169.0.0", 8080) && !client.connected()) {
    Serial.println("TCP Connection in progress...");
    if((endtime - start_time) >= 10000) {
      Serial.println("Failed connecting to server " + "192.168.0.0:" + 8080); // We will edit the server ip and port to become variables later on.
      client.stop();
      while (1);
    }
    end_time = millis();
  }
}

void loop() {
  
  // check signal from desktop and send to all microcontrollers

  // if packet from desktop, send corresponding can packet to controllers
  // figure out how to get tcp packets!!
  
  // save packet to sd card
  packetStore.println("tcp packet");
  
  CAN.beginPacket(0x12);
  CAN.write('');
  CAN.endPacket();
  
  // if packet from controller, send tcp to desktop
  int packetSize = CAN.parsePacket();

  if (packetSize) {
    String data;
    
    while (CAN.available()) {
      char c = (char)CAN.read();
      data += c;
    }   
     
    // save packet to sd card
    packetStore.println("CAN packet");
    
    // create a tcp packet
    // figure out how to send tcp packet to desktop!

    



  }

  packetStore.close();
}
