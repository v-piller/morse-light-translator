#include "M5Atom.h"
// Declare the color variable as a global variable using the correct type.
uint32_t color = 0x000000;

void setup() {
  M5.begin(true, false, true);
  delay(50);
  M5.dis.drawpix(0, 0x00ff00);
}

void loop() {
  M5.dis.drawpix(0, color); //alume la led avec la color
  delay(1);
  color = color + 0x000050; // augmente la valeur de la color pour se rapprocher du blanc
}
