#include <stdio.h>
#include "morse.h"

int main() {
    const char* phrase = "IL FAIT BEAU AUJOURD'HUI";
    char morse[1024];
    char texte[256];

    // Encodage
    text_to_morse(phrase, morse);
    printf("Texte original : %s\n", phrase);
    printf("Morse          : %s\n", morse);

    // Décodage
    morse_to_text(morse, texte);
    printf("Texte décodé   : %s\n", texte);

    return 0;
}
