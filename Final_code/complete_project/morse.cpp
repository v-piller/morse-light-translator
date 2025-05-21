#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "morse.h"

/* 
 * Function morse_to_index by cypherpunks on Reddit.
 * See: http://goo.gl/amr6A3
 */
int morse_to_index (const char* str)
{
        unsigned char sum = 0, bit;

        for (bit = 1; bit; bit <<= 1) {
                switch (*str++) {
                case 0:
                        return sum | bit;
                default:
                        return 0;
                case '-':
                        sum |= bit;
                        /* FALLTHROUGH */
                case '.':
                        break;
                }
        }

        return 0;
}

const char* char_to_morse (char c)
{
        if (islower(c))
                c += ('A' - 'a');

        return CHAR_TO_MORSE[(int) c];
}

const char* morse_to_char (const char* str)
{
        return MORSE_TO_CHAR[morse_to_index(str)];
}
void text_to_morse(const char* text, char* out) {
        out[0] = '\0';  // on vide le buffer
    
        for (size_t i = 0; i < strlen(text); i++) {
            if (text[i] == ' ') {
                strcat(out, "/ ");  // convention : '/' entre mots
            } else {
                const char* code = char_to_morse(text[i]);
                if (code != NULL) {
                    strcat(out, code);
                    strcat(out, " ");  // espace entre lettres
                }
            }
        }
    }
    
    void morse_to_text(const char* morse, char* out) {
        char copy[1024];
        strcpy(copy, morse);
    
        out[0] = '\0';
        char* token = strtok(copy, " ");
        while (token != NULL) {
            if (strcmp(token, "/") == 0) {
                strcat(out, " ");
            } else {
                const char* letter = morse_to_char(token);
                if (letter != NULL) {
                    strcat(out, letter);
                }
            }
            token = strtok(NULL, " ");
        }
    }