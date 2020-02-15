#include <CAN.h>
#include <SPI.h>
#include <SD.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

int BUFFER_SIZE_CAN = 8;
File dataLog;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// IP address will be dependent on local network
IPAddress ip(169, 254, 167, 62);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

// UDP CONFIGURATIONS
unsigned int localPort = 23;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
EthernetUDP Udp;
char ReplyBuffer[] = "acknowledged";




void setup() {
  // CONFIGURE THE CS PIN FOR THE EHTERNET SHIELD
  Ethernet.init(10);

  
  Serial.begin(9600);
  while (!Serial);

  Ethernet.begin(mac, ip, gateway, subnet);
  
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  Udp.begin(localPort);

  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed.");
  }

  if (!SD.begin(4)) {
    Serial.println("SD card init failed.");
  }
  
  dataLog = SD.open("dataLog.txt", FILE_WRITE); // Filename must be <= 8 characters long.

  // register callback for when CAN packet is received
  CAN.onReceive(onReceive);
}

void loop() {
  
  // *** TODO: check signal from desktop and send to all microcontrollers
  // 

  // if packet is received from desktop, send can packet to controllers
  
  // Check for available data, and read
  int packetSize = Udp.parsePacket();
  
  if (packetSize) { // packetSize > 0
     // Read packet into buffer
     Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
     Serial.print("[UDP]: ");
     Serial.println(packetBuffer);

     
     
    
     // save UDP packet to sd card
     if (dataLog) {
       dataLog.println("UDP: ");
       dataLog.print("[id: ");
       dataLog.print(m);
       dataLog.print(", data: ");
       dataLog.print(buf);
       dataLog.print("]");
       dataLog.println();
       dataLog.flush();
     } else {
       Serial.println("Error opening dataLog.txt");
     }

      // create and send CAN packet
      // packet has maximum size of 8 bytes
  
     // create CAN packet
     Serial.println("Starting CAN packet");
     CAN.beginPacket(messageId);
     CAN.write(buf, BUFFER_SIZE_CAN);
     CAN.endPacket();
     Serial.println("Sent CAN Packet");
     delay(1000);
   }
  Serial.println("Client disconnected.");
}




// if packet is received from controller, read data and send tcp to desktop
void onReceive(int packetSize) {
  Serial.print("Received CAN packet of size ");
  Serial.print(packetSize);
  Serial.println();
  
  char data[BUFFER_SIZE_CAN];

  CAN.readBytes(data, BUFFER_SIZE_CAN);
  Serial.print("CAN: ");
  Serial.print("[id: ");
  Serial.print(CAN.packetId(), HEX);
  Serial.print(", data: ");
  Serial.print(data);
  Serial.print("]");
  Serial.println();
     
  // save packet to sd card
  if (dataLog) {
    Serial.println("Saving CAN packet to log.");
    dataLog.print("CAN: ");
    dataLog.print("[id: ");
    dataLog.print(CAN.packetId(), HEX);
    dataLog.print(", data: ");
    dataLog.print(data);
    dataLog.print("]");
    dataLog.println();
    dataLog.flush();
  } else {
    Serial.println("Error opening dataLog.txt");
  }

  if (client.connected()) {
    // create a tcp packet
    Serial.println("Creating TCP packet.");
    client.write(CAN.packetId());
    client.write(data, sizeof(data));
    data[0] = '\0';
    Serial.println("Sent TCP packet");
    // TODO: wait for confirmation that packet was received. if no response, resent packet until response arrives
  }
}