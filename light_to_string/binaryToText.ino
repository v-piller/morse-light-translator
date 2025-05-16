#include "morse.h"
#include <stdio.h>
#include <stddef.h>
#include <M5Atom.h>


#define DOT_TIME 200
#define DASH_TIME 600
#define GAP_LETTER 600
#define GAP_WORD 1400
#define TOLERANCE 50


// message should say "Hello"
char message[] = "1:200/0:200/1:200/0:200/1:200/0:200/1:200/0:600/1:200/0:600/1:200/0:200/1:600/0:200/1:200/0:200/1:200/0:600/1:200/0:200/1:600/0:200/1:200/0:200/1:200/0:600/1:600/0:200/1:600/0:200/1:600";
char textOut[512];

//Checks wheter a number is close enough to a target
//Return true if the value is between (target - tolerance) and (target + tolerance)
bool approx(int value, int target) {
  return value >= target - TOLERANCE && value <= target + TOLERANCE;
}


void binaryToText(const char* inputBinaryString, char* outputText) {
  // to store the converted Morse string
  char morseOut[512];
  morseOut[0] = '\0';  // clear the buffer

  // create new array to copy original string so it can be safely modified (same as in morse.cpp)
  char inputCopy[1024];

  //We copy the message into inputCopy with max size of 1024 (otherwise--> buffer overflow)
  strncpy(inputCopy, inputBinaryString, sizeof(inputCopy));
  inputCopy[sizeof(inputCopy) - 1] = '\0'; // this way we make sure the copied string is null terminated


  //We separate inputCopy by "/" leaving us with (strtok() gets one part at a time and saves it in token)
  char* token = strtok(inputCopy, "/");

  //as long as we have pieces left in the string to process, do ...
  while (token != NULL) {
    //in char* separation we save ":", the token is NULL if we don't find ":" so ex: "1:200/NULL/1:600"
    char* separation = strchr(token, ':');
    if (!separation) {
      token = strtok(NULL, "/");
      continue;
    }

    *separation = '\0'; //we split string at ":"
    int state = atoi(token); //atoi() turns token (1 or 0) into an integer
    int duration = atoi(separation + 1); //takes what's after separation and also turn it into an integer


    if (state == 1) {
      if (approx(duration, DOT_TIME)) {
        strcat(morseOut, ".");
      } else if (approx(duration, DASH_TIME)) {
        strcat(morseOut, "-");
      }

    } else if (state == 0) {
      if (approx(duration, GAP_LETTER)) {
        strcat(morseOut, " ");
      } else if (approx(duration, GAP_WORD)) {
        strcat(morseOut, "/");
      } // we do nothing if gap_intra_char ?

    }

    //We take the next token (also using "/" ofc)
    token = strtok(NULL, "/");

  }


  Serial.print("Message in morse: ");
  Serial.println(morseOut);

  morse_to_text(morseOut, textOut);


}



void setup() {
  M5.begin(true, false, true);
  Serial.begin(115200);
  
  binaryToText(message, textOut);

  Serial.print("Final message: ");
  Serial.println(textOut);
}

void loop() {
  // put your main code here, to run repeatedly:

}
