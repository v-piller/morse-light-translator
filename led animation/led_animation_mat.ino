#include "M5Atom.h"

uint32_t currentColor = 0x0000FF;  // Commence avec du bleu
unsigned long previousMillis = 0;
const long interval = 1000;

void setup() {
  M5.begin(true,false,true);
  delay(50);
  M5.dis.drawpix(0, currentColor);
  

}

void loop() {
    unsigned long currentMillis = millis();

    if(currentMillis - previousMillis >= interval){
      previousMillis = currentMillis;

    // Extraire les composantes
    uint8_t r = (currentColor >> 16) & 0xFF;
    uint8_t g = (currentColor >> 8) & 0xFF;
    uint8_t b = currentColor & 0xFF;
    // Rotation des composantes RGB
    uint8_t temp = r;
    r = g;
    g = b;
    b = temp;
    
    // Recomposer la couleur
    currentColor = (r << 16) | (g << 8) | b;
    
    M5.dis.drawpix(0, currentColor);


    }

}
