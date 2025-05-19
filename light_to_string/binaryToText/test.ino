#include "morse.h"
#include "binaryToText.h" 
#include <stdio.h>
#include <stdlib.h>
#include <M5Atom.h>

char message[] =
  "1:200/0:200/1:200/0:200/1:200/0:200/1:200/0:600/"      // H
  "1:200/0:600/"                                           // E
  "1:200/0:200/1:600/0:200/1:200/0:200/1:200/0:600/"       // L
  "1:200/0:200/1:600/0:200/1:200/0:200/1:200/0:600/"       // L
  "1:600/0:200/1:600/0:200/1:600/0:1400/"                  // O
  "1:200/0:200/1:600/0:200/1:600/0:600/"                   // W
  "1:600/0:200/1:600/0:200/1:600/0:600/"                   // O
  "1:200/0:200/1:600/0:200/1:200/0:600/"                   // R
  "1:200/0:200/1:600/0:200/1:200/0:200/1:200/0:600/"       // L
  "1:600/0:200/1:200/0:200/1:200";                         // D

char outputText[512];

void setup() {
  Serial.begin(115200);

  binaryToText(message, outputText);

  Serial.print("Texte converti : ");
  Serial.println(outputText);
}

void loop() {
  // Rien à répéter ici
}
