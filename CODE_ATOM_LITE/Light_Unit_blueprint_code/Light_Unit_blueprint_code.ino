/*
 * Unit-LIGHT sensor on Atom LITE
 * Analog  → GPIO32 (white wire)  – 0-4095
 * Digital → GPIO26 (yellow wire) – 0 / 1
 */

#include <M5Atom.h>     // Atom-specific wrapper (pulls in FastLED)
#include <FastLED.h>    // For CRGB and the NeoPixel macro

void setup() {
  // M5.begin(EnableSerial, EnableI2C, EnableDisplayRGB)
  M5.begin(true, false, true);
  Serial.begin(115200); // 115200 baud (Serial Monitor)

  pinMode(26, INPUT);   // digital output from sensor
  // pinMode(32, INPUT); // optional – analogRead works without this
}

void loop() {
  uint16_t aValue = analogRead(32);   // 0-4095 light level
  uint8_t  dValue = digitalRead(26);  // threshold output

  Serial.printf("Analog: %4u  Digital: %u\n", aValue, dValue);

  // Convert 0-4095 ADC range to 0-255 brightness for the RGB LED
  uint8_t level = map(aValue, 0, 4095, 0, 255);
  // show a white LED whose intensity follows the light sensor
  M5.dis.drawpix(0, CRGB(level, level, level));

  delay(50);
}