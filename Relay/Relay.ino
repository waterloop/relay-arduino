#include <CAN.h>
#include <SPI.h>
#include <SD.h>
#include <Ethernet.h>

int BUFFER_SIZE_CAN = 8;
int BUFFER_SIZE_TCP = 8;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
File dataLog;
IPAddress ip(192, 168, 0, 62);
EthernetServer server(23);
EthernetClient client;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  Ethernet.begin(mac, ip);
  server.begin();
  
  while(Ethernet.hardwareStatus() == EthernetNoHardware)Serial.println("Ethernet shield was not found.");
  while(Ethernet.linkStatus() == LinkOFF)Serial.println("Ethernet cable is not connected.");
  while(!SD.begin(4))Serial.println("SD.begin failed.");
  while(!CAN.begin(500E3)) ;
  
  dataLog = SD.open("dataLog.txt", FILE_WRITE); // Filename must be <= 8 characters long.
  
  Serial.println("Init successful.");
  
  Serial.println("Waiting for client to connect.");
  while (!(client = server.accept())) {}
  Serial.println("Desktop connected.");
  
}

void loop() {
  if (client.available()) {
    char buf[BUFFER_SIZE_TCP];
    char messageId = client.read();
    client.read(buf, BUFFER_SIZE_TCP);
    Serial.print("TCP: [id: ");
    Serial.print(messageId);
    Serial.print(", data: ");
    Serial.print(buf);
    Serial.print("]\n");
    // save tcp packet to sd card
    if (dataLog) {
      dataLog.print("TCP: [id: ");
      dataLog.print(messageId);
      dataLog.print(", data; ");
      dataLog.print(buf);
      dataLog.print("]\n");
      dataLog.flush();
    } else {
      Serial.println("Error opening dataLog.txt");
    }
    
    // create and send CAN packet
    // packet has maximum size of 8 bytes

    // create CAN packet
    //Serial.println("Starting CAN packet");
    CAN.beginPacket(messageId);
    CAN.write(buf, BUFFER_SIZE_CAN);
    CAN.endPacket();
    Serial.println("Sent CAN");     
    delay(100);
  }
  
  int packetSize = CAN.parsePacket();
  if (packetSize) {
    Serial.println("Received CAN");
    char data[BUFFER_SIZE_CAN];
    CAN.readBytes(data, BUFFER_SIZE_CAN);
    Serial.print("CAN: [id: ");
    Serial.print(CAN.packetId(), HEX);
    Serial.print(", data: ");
    Serial.print(data);
    Serial.print("]\n");

    // save packet to sd card
    if (dataLog) {
      dataLog.print("CAN: [id: ");
      dataLog.print(CAN.packetId(), HEX);
      dataLog.print(", data: ");
      dataLog.print(data);
      dataLog.print("]\n");
      dataLog.flush();
    } else {
      Serial.println("Error opening dataLog.txt");
    }
  
    if (client.connected()) {
      // create a tcp packet
      client.write(CAN.packetId());
      client.write(data, sizeof(data));
      data[0] = '\0';
      Serial.println("Sent TCP");
      // TODO: wait for confirmation that packet was received. if no response, resend packet until response arrives
    }
  }
}