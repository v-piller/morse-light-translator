#include "morse.h"
#include "string_to_light.h"

#include <stdio.h>
#include <stddef.h>
#include <M5Atom.h>
char* message = "SOS";
void setup() {
  M5.begin(true, false, true); // Atom initialization
  pinMode(FLASH_PIN, OUTPUT);
  digitalWrite(FLASH_PIN, LOW);

  Serial.begin(115200);

  transmitMessage(message);
}

void loop() {
  // do nothing
}