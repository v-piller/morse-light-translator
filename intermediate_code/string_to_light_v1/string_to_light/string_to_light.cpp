#include "string_to_light.h"
#include "morse.h"
#include <stdio.h>
#include <stddef.h>

#define FLASH_PIN 26  // Built-in LED on M5Atom

const int DOT_TIME = 200;             // 200 ms for a dot
const int DASH_TIME = 3 * DOT_TIME;   // 600 ms for a dash (3 units)
const int GAP_INTRA_CHAR = DOT_TIME;  // 200 ms gap between parts of the same letter
const int GAP_LETTER = 2 * DOT_TIME;  // 600 ms gap between letters, minus one already include break
const int GAP_WORD = 6 * DOT_TIME;    // 1400 ms gap between words, minus one already include break




//For dot (.), short flash:
void transmitDot() {
  digitalWrite(FLASH_PIN, HIGH);    // LED on
  delay(DOT_TIME);
  digitalWrite(FLASH_PIN, LOW);     // LED off (dot completed)
  delay(GAP_INTRA_CHAR);           // brief gap after dot (character space)
}

//For dash (-), long flash:
void transmitDash() {
  digitalWrite(FLASH_PIN, HIGH);
  delay(DASH_TIME);
  digitalWrite(FLASH_PIN, LOW);
  delay(GAP_INTRA_CHAR);
}

//make lights go off and on with the whole string/message already in morse
void transmitMessage(const char* text) {
    char morseBuf[512];               // Buffer stays local to this function
    text_to_morse(text, morseBuf);
    Serial.print("Morse message: ");
    Serial.println(morseBuf);          // Print Morse for debugging

    for (int i = 0; morseBuf[i] != '\0'; ++i) {
        char c = morseBuf[i];
        if (c == '.') transmitDot();
        else if (c == '-') transmitDash();
        else if (c == ' ') {
            // end of letter
            delay(2*DOT_TIME);
        }
        else if (c == '/') {
            // end of word
            delay(6*DOT_TIME);
        }
    }
}
