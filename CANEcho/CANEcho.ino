// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <CAN.h>

int i = 0;
char* s;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("CAN Sender");

  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }

  s = malloc(128);
}

void loop() {
  int lenCAN = CAN.parsePacket();
  if(lenCAN > 0) {
    Serial.print(lenCAN);
    CAN.beginPacket(0x12);
    int len = sprintf(s, "r%d", lenCAN);
    CAN.write(s, len);
    CAN.endPacket();
  }
}
