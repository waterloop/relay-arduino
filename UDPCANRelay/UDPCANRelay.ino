#include <SPI.h>
#include <SD.h>
#include <Ethernet.h>
#include <CAN.h>

int BUFFER_SIZE_CAN = 8;
int BUFFER_SIZE_CAN_ID = 2;
int BUFFER_SIZE_TCP = 8;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress relayIp(192, 168, 0, 9);
int relayPort = 2323;
IPAddress desktopIp(192, 168, 0, 6);
int desktopPort = 2323;

unsigned char buf[UDP_TX_PACKET_MAX_SIZE];

EthernetUDP Udp;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Ethernet.begin(mac, relayIp);
  while(!Udp.begin(relayPort)) Serial.println("Udp.begin failed");
  while(!CAN.begin(500E3)) Serial.println("CAN.begin failed");
  while(!SD.begin(4)) Serial.println("SD.begin failed");
  Serial.println("Successful init");
}

void loop() {
  int packetSize = Udp.parsePacket();
  File sdlog;
  if(packetSize) {
    // assert Udp.remoteIP() == desktopip
    // assert Udp.remotePort() == desktopport
    if (BUFFER_SIZE_CAN_ID < packetSize && packetSize <= BUFFER_SIZE_CAN + BUFFER_SIZE_CAN_ID) { // note that we do not allow zero-length CAN packets
      Udp.read(buf, UDP_TX_PACKET_MAX_SIZE);
      uint16_t messageId = *((uint16_t*)buf); // Type depends on BUFFER_SIZE_CAN_ID
      
      Serial.println("received udp");
      Serial.println(messageId);
      if (messageId >= (1<<11)) {
        Serial.println("WARNING: message id too big, truncating");
        messageId &= (1<<11) - 1;
      }
      Serial.println(packetSize - BUFFER_SIZE_CAN_ID);
      
      CAN.beginPacket(messageId);
      CAN.write(buf + BUFFER_SIZE_CAN_ID, packetSize - BUFFER_SIZE_CAN_ID);
      CAN.endPacket();

      sdlog = SD.open("log.txt", FILE_WRITE); // Filename must be <= 8 characters long.
      sdlog.write("UDP ");
      sdlog.write(buf, packetSize);
      sdlog.write('\n');
      Serial.println("Wrote UDP to SD card!!");
      sdlog.close();
    } else {
      Serial.println("received udp bad size");
    }
  }

  packetSize = CAN.parsePacket();
  if(packetSize) {
    Serial.println("received can");
    
    Udp.beginPacket(desktopIp, desktopPort);
    uint16_t packetId = (uint16_t)CAN.packetId(); // Type depends on BUFFER_SIZE_CAN_ID
    Udp.write((uint8_t*)&packetId, BUFFER_SIZE_CAN_ID);
    CAN.readBytes(buf, packetSize);
    Udp.write(buf, packetSize);
    Udp.endPacket();
    
    sdlog = SD.open("log.txt", FILE_WRITE); // Filename must be <= 8 characters long.
    sdlog.write("CAN ");
    sdlog.write((uint8_t*)&packetId, BUFFER_SIZE_CAN_ID);
    sdlog.write((uint8_t*)buf, packetSize);
    sdlog.write('\n');
    Serial.println("Wrote CAN to SD card!!");
    sdlog.close();
  }
}
