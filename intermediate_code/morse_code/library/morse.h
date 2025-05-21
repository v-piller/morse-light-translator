#ifndef _morse_h
#define _morse_h

static const char* CHAR_TO_MORSE[128] = {
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, "-.-.--", ".-..-.", NULL, NULL, NULL, NULL, ".----.",
        "-.--.", "-.--.-", NULL, NULL, "--..--", "-....-", ".-.-.-", "-..-.",
        "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...",
        "---..", "----.", "---...", NULL, NULL, "-...-", NULL, "..--..",
        ".--.-.", ".-", "-...", "-.-.", "-..", ".", "..-.", "--.",
        "....", "..", ".---", "-.-", ".-..", "--", "-.", "---",
        ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--",
        "-..-", "-.--", "--..", NULL, NULL, NULL, NULL, "..--.-",
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
};

static const char* MORSE_TO_CHAR[128] = {
        NULL, NULL, "E", "T", "I", "N", "A", "M",
        "S", "D", "R", "G", "U", "K", "W", "O",
        "H", "B", "L", "Z", "F", "C", "P", NULL,
        "V", "X", NULL, "Q", NULL, "Y", "J", NULL,
        "5", "6", NULL, "7", NULL, NULL, NULL, "8",
        NULL, "/", NULL, NULL, NULL, "(", NULL, "9",
        "4", "=", NULL, NULL, NULL, NULL, NULL, NULL,
        "3", NULL, NULL, NULL, "2", NULL, "1", "0",
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, ":",
        NULL, NULL, NULL, NULL, "?", NULL, NULL, NULL,
        NULL, NULL, "\"", NULL, NULL, NULL, "@", NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, "'", NULL,
        NULL, "-", NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, ".", NULL, "_", ")", NULL, NULL,
        NULL, NULL, NULL, ",", NULL, "!", NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

const char* char_to_morse(char);
const char* morse_to_char(const char*);
int morse_to_index (const char*);
void text_to_morse(const char* text, char* out);
void morse_to_text(const char* morse, char* out);

#endif