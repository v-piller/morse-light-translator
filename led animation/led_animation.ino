#include <M5Atom.h>


// we define 3 different colors usimg RGB hex values
#define RED     0xFF0000
#define PURPLE  0x8000FF
#define BLUE    0x0000FF

void setup() {
  // Initialize the processor
  M5.begin(true, false, true);
}

// Function that fills the entire LED matric with a single color
void fillMatrix(uint32_t color) {
  for (int i = 0; i < 25; i++) {
    // Set the color of each pixel (from 0 to 24)
    M5.dis.drawpix(i, color);
  }
}

void loop() {
  // Cycle through 3 different colors, changing every 300 milliseconds
  fillMatrix(RED);    delay(300); // Show red
  fillMatrix(PURPLE); delay(300); // Show purple
  fillMatrix(BLUE);   delay(300); // Show blue
}
