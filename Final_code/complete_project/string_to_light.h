#ifndef STRING_TO_LIGHT_H
#define STRING_TO_LIGHT_H


//uses Arduino functions like digitalWrite, delay, Serial.
#include <M5Atom.h>

// Define the pin for the LED.
#define FLASH_PIN 25

// Morse code timing constants (in milliseconds).
extern const short DOT_TIME;         // Duration of a dot
extern const short DASH_TIME;        // Duration of a dash (typically 3 * DOT_TIME)
extern const short GAP_INTRA_CHAR;   // Gap between dots/dashes within the same character (typically DOT_TIME)
extern const short GAP_LETTER;       // Gap between letters (typically 2 * DOT_TIME, after intra-char gap)
extern const short GAP_WORD;         // Gap between words (typically 7 * DOT_TIME, after intra-char gap)

/**
 * @brief Transmits a Morse code dot.
 * Turns the LED on for DOT_TIME, then off, then waits for GAP_INTRA_CHAR.
 * Assumes FLASH_PIN is configured as an output.
 */
void transmitDot(void);

/**
 * @brief Transmits a Morse code dash.
 * Turns the LED on for DASH_TIME, then off, then waits for GAP_INTRA_CHAR.
 * Assumes FLASH_PIN is configured as an output.
 */
void transmitDash(void);

/**
 * @brief Transmits a given text string as Morse code using an LED.
 *
 * This function first converts the input text to its Morse code representation
 * (e.g., "SOS" -> "... --- ...") using an external `text_to_morse` function.
 * This `text_to_morse` function must be provided and declared elsewhere (e.g., via "morse.h"
 * which you included in your original C file).
 *
 * It then iterates through the Morse string, flashing the LED connected to FLASH_PIN
 * for dots and dashes, and inserting appropriate delays for inter-element,
 * inter-character, and inter-word gaps.
 *
 * @param text The null-terminated C string to transmit (e.g., "HELLO WORLD").
 *
 * @note This function relies on `text_to_morse(const char* inputText, char* morseBuffer)`
 * being available and linked from your project (e.g. from your "morse.h" and its
 * corresponding implementation).
 * @note The implementation of this function (in the .cpp file) uses `Serial.print`
 * and `Serial.println` for debugging output. Ensure Serial communication is
 * initialized in your `setup()` function (e.g., `Serial.begin(9600);`) if you
 * want to see these messages.
 * @note Remember to set `pinMode(FLASH_PIN, OUTPUT);` in your `setup()` function.
 */
void transmitMessage(const char* text);

#endif //STRING_TO_LIGHT_H
