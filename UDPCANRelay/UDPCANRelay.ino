#include <SPI.h>
#include <SD.h>
#include <Ethernet.h>
#define protected public // I'm sorry
#include <CAN.h>

int BUFFER_SIZE_CAN = 8;
int BUFFER_SIZE_TCP = 8;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress relayip(192, 168, 0, 9);
int relayport = 2323;
IPAddress desktopip(192, 168, 0, 255);
int desktopport = 2323;

unsigned char buf[UDP_TX_PACKET_MAX_SIZE];
unsigned char sprintfbuf[24];

EthernetUDP Udp;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Ethernet.begin(mac, relayip);
  while(!Udp.begin(relayport))Serial.println("Udp.begin failed");
  while(!CAN.begin(500E3))Serial.println("CAN.begin failed");
  while(!SD.begin(4))Serial.println("SD.begin failed");
  Serial.println("Successful init");
}

void loop() {
  int packetSize = Udp.parsePacket();
  File sdlog;
  if(packetSize) {
    // assert Udp.remoteIP() == desktopip
    // assert Udp.remotePort() == desktopport
    if (4 < packetSize && packetSize <= 12) { // note that we do not allow zero-length CAN packets
      Udp.read(buf, UDP_TX_PACKET_MAX_SIZE);
      uint32_t messageId = *((uint32_t*)buf);
      
      Serial.println("received udp");
      Serial.println(messageId);
      if (messageId >= (1<<11)) {
        Serial.println("WARNING: message id too big, truncating");
        messageId &= (1<<11) - 1;
      }
      Serial.println(packetSize - 4);
      
      CAN.beginPacket(messageId);
      CAN.write(buf + 4, packetSize - 4);
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
    
    Udp.beginPacket(desktopip, desktopport);
    uint32_t packetId = (uint32_t)CAN.packetId();
    Udp.write((uint8_t*)&packetId, 4);
    Udp.write(CAN._rxData, packetSize);
    Udp.endPacket();
    
    sdlog = SD.open("log.txt", FILE_WRITE); // Filename must be <= 8 characters long.
    sdlog.write("CAN ");
    sdlog.write((uint8_t*)&packetId, 4);
    sdlog.write((uint8_t*)CAN._rxData, packetSize);
    sdlog.write('\n');
    Serial.println("Wrote CAN to SD card!!");
    sdlog.close();
  }
}
