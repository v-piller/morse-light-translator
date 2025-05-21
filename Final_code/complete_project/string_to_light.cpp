#include "string_to_light.h"
#include "morse.h"
#include <stdio.h>
#include <stddef.h>

const short DOT_TIME = 200;             // 200 ms for a dot
const short DASH_TIME = 3 * DOT_TIME;   // 600 ms for a dash (3 units)
const short GAP_INTRA_CHAR = DOT_TIME;  // 200 ms gap between parts of the same letter
const short GAP_LETTER = DOT_TIME * 2; // 400 ms to add to the existing 200 ms
const short GAP_WORD = 2 * DOT_TIME;    // add 400 ms to reach 1400




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

    for (int8_t i = 0; morseBuf[i] != '\0'; ++i) {
        char c = morseBuf[i];
        if (c == '.') transmitDot();
        else if (c == '-') transmitDash();
        else if (c == ' ') {
            // end of letter
            delay(GAP_LETTER);
        }
        else if (c == '/') {
            // end of word
            delay(GAP_WORD);
        }
    }
}
