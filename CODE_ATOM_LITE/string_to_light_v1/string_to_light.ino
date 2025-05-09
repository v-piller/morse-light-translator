#include "morse.h"
#include <stdio.h>
#include <stddef.h>
#include <M5Atom.h>

#define FLASH_PIN 26  // Built-in LED on M5Atom

const int DOT_TIME = 200;             // 200 ms for a dot
const int DASH_TIME = 3 * DOT_TIME;   // 600 ms for a dash (3 units)
const int GAP_INTRA_CHAR = DOT_TIME;  // 200 ms gap between parts of the same letter
const int GAP_LETTER = 3 * DOT_TIME;  // 600 ms gap between letters
const int GAP_WORD = 7 * DOT_TIME;    // 1400 ms gap between words


char* message = "My name is Valeria.";
char morse[512];


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
    text_to_morse(message, morseBuf);
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

/*
int main (void) {
    text_to_morse(message, morse);
    printf("Message: %s\n", text);
    printf("Message in morse code: %s\n", morse);


    transmitMessage(morse);
    //morse_to_text(morse, text);
    //printf("Morse: %s\n", morse);
    //printf("Final message: %s\n", text);

    return 0;
}
*/

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