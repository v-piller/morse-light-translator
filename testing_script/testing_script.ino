#include "M5Atom.h"     // For M5Atom functionalities (M5.begin(), etc.)
// Custom libraries
#include "wifi_and_messages.h"  // Handles Wi-Fi connection, mDNS, and HTTP communication
#include "string_to_light.h"    // Handles Morse transmission via LED (uses morse.h)
#include "morse.h"              // Handles text <-> morse conversion (used by string_to_light)
#include "binaryToText.h"
#include "flash_detector.h"
#include "history.h"

bool isTestPositive = true;
const char* ssid = "SURFACE-JEREMIE 2090";
const char* password = ">40Fc667";

bool approx(int val, int target, int tol = 50) {
  return val >= target - tol && val <= target + tol;
}

bool testFlashDetectionAndCompare() {
    Serial.println("=== Starting flash detection test ===");
    int count = 0;
    // Call processFlashDetection in a loop until a complete sequence is obtained
    String result = "";
    int attempts = 0;
    while (result.length() == 0 && attempts < 100000) {
      result = processFlashDetection();
      if (count < result_index) {
          count = result_index;
      }
      attempts++;
  }
  if (result.length() == 0) {
      Serial.println("Timeout: no sequence detected.");
      return false;
  }

    Serial.println("Sequence detected: " + result);

    
    FlashEvent* events = flash_table;


    // Comparison with the expected sequence (example for 'test' in Morse)
    FlashEvent expected[] = {
        {1, 600}, {0, 600}, {1, 200}, {0, 600},
        {1, 200}, {0, 200}, {1, 200}, {0, 200}, {1, 200}, {0,600}, {1,600}
    };

    bool match = true;
    if (count != sizeof(expected) / sizeof(expected[0])) {
        match = false;
        Serial.println("Error: sizes do not match");
        Serial.println(count);
        Serial.println(sizeof(expected) / sizeof(expected[0]));
    } else {
        for (int i = 0; i < count; i++) {
            if (events[i].state != expected[i].state || !approx(events[i].duration, expected[i].duration)) {
              Serial.println(events[i].state);
              Serial.println(events[i].duration);
                match = false;
                break;
            }
        }
    }

    if (match) {
        Serial.println("The sequence matches the word 'test'.");
        return true;
    } else {
        Serial.println("The sequence does NOT match.");
        return false;
    }

    Serial.println("=== End of sensor test ===\n");
}

void sendFlashesTask(void* parameter) {
  transmitMessage("test");
  vTaskDelete(NULL);
}

void testHistoryFunctions() {
  Serial.println("=== History (ORB) Test ===");

  OverwritingRingBuffer rb;
  ORB_init(&rb);

  // Test: Buffer should be empty after initialization
  if (!ORB_isEmpty(&rb)) {
    Serial.println("Error: buffer is not empty after initialization.");
    isTestPositive = false;
  } else {
    Serial.println("Buffer is empty after initialization.");
  }

  // Create a test message
  Message msg1 = {"HELLO", true, time(NULL)};
  ORB_push(&rb, &msg1);

  if (ORB_isEmpty(&rb)) {
    Serial.println("Error: buffer is empty after insertion.");
    isTestPositive = false;
  } else {
    Serial.println("Buffer is not empty after insertion.");
  }

  // Test: size should be 1
  if (ORB_size(&rb) != 1) {
    Serial.print("Incorrect size: ");
    Serial.println(ORB_size(&rb));
    isTestPositive = false;
  } else {
    Serial.println("Correct size (1).");
  }

  // Push more messages to fill the buffer
  for (int i = 0; i < ORB_CAPACITY; i++) {
    Message m = {"MSG", false, time(NULL) + i};
    ORB_push(&rb, &m);
  }

  if (!ORB_isFull(&rb)) {
    Serial.println("Buffer should be full.");
    isTestPositive = false;
  } else {
    Serial.println("Buffer is full after filling.");
  }

  // Test overwriting
  Message overwritten = {"OVERWRITE", true, time(NULL)};
  ORB_push(&rb, &overwritten);

  Message popped;
  ORB_pop(&rb, &popped);

  Serial.print("First message after overflow: ");
  Serial.println(popped.text);

  // Test clearing
  ORB_clear(&rb);
  if (!ORB_isEmpty(&rb)) {
    Serial.println("Buffer is not empty after ORB_clear().");
    isTestPositive = false;
  } else {
    Serial.println("Buffer is empty after ORB_clear().");
  }

  Serial.println("=== End of history test ===\n");
}

void testMorseConversion() {
  Serial.println("=== Test Morse Conversion Functions ===");

  // Test 1: char_to_morse and morse_to_char
  char testChar = 'S';
  const char* morse = char_to_morse(testChar);
  Serial.print("Morse for '");
  Serial.print(testChar);
  Serial.print("': ");
  Serial.println(morse);

  const char* reverseChar = morse_to_char(morse);
  Serial.print("Char for \"");
  Serial.print(morse);
  Serial.print("\": ");
  Serial.println(reverseChar);

  if (!morse || !reverseChar || reverseChar[0] != testChar) {
    Serial.println("char_to_morse or morse_to_char failed");
    isTestPositive = false;
  } else {
    Serial.println("char_to_morse and morse_to_char passed");
  }

  // Test 2: text_to_morse
  const char* inputText = "SOS";
  char morseOutput[128];
  text_to_morse(inputText, morseOutput);
  Serial.print("Morse for \"SOS\": ");
  Serial.println(morseOutput);

  // Test 3: morse_to_text
  char recoveredText[32];
  morse_to_text(morseOutput, recoveredText);
  Serial.print("Text for Morse: ");
  Serial.println(recoveredText);

  if (strcmp(inputText, recoveredText) != 0) {
    Serial.println("text_to_morse or morse_to_text failed");
    isTestPositive = false;
  } else {
    Serial.println("text_to_morse and morse_to_text passed");
  }

  Serial.println("=== End of Morse Conversion Tests ===\n");
}

void testWiFiConnection() {
  Serial.println("== WiFi Test ==");

  wifi_setup(ssid, password);
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connection successful");
  } else {
    Serial.println("WiFi connection failed");
    isTestPositive = false;
  }
}

void testServerDiscovery() {
  Serial.println("== Server Discovery Test ==");

  ServerDetails server = server_setup();
  if (server.found) {
    Serial.print("Server found at ");
    Serial.print(server.ip);
    Serial.print(":");
    Serial.println(server.port);
  } else {
    Serial.println("Server not found");
    isTestPositive = false;
  }
}

void testMessageSendReceive() {
  Serial.println("== Message Send/Receive Test ==");

  ServerDetails server = server_setup();
  if (!server.found) {
    Serial.println("Server not available");
    isTestPositive = false;
    return;
  }

  sendMessage(server, "Test message from ESP32");
  delay(1000);    // Give server time to process

  String msg = receiveMessage(server);
  if (msg.length() > 0) {
    Serial.print("Message received: ");
    Serial.println(msg);
    Serial.print("Should have been empty as the server should sent back the same message");
    isTestPositive = false;
  } else {
    Serial.println("Empty message has arrived, as expected");
    
  }
}

void setup(){
  Serial.begin(115200);
  M5.begin(true, false, true);  // Initialize M5Atom (Serial, I2C, Display enabled)
  pinMode(FLASH_PIN, OUTPUT);   // Defined in string_to_light.h as G26
  digitalWrite(FLASH_PIN, LOW); // Ensure LED is off at startup
  pinMode(SENSOR_PIN, INPUT);   // Configure sensor pin as input (uses library constant)
  Serial.println("Testing Script for our morse-code system");
  Serial.println("Flashlight should shortly blink once, then blink a longer time");
  transmitDot();
  delay(1000);
  transmitDash();
  Serial.println("If the flashlight did not work, please verify that it is plugged in correctly. Data should be on pin G25.");
  Serial.println("The flashlight will blink \"SOS S O S\". Please verify that it follows international Morse code conventions.");
  transmitMessage("SOS S O S");
  Serial.println("Note: Since there is no feedback from the flashlight, we assume it worked as expected.");
  Serial.println("The light sensor will now calibrate twice, with a 10-second delay between each calibration.");
  Serial.println("Please ensure the results are consistent if done under the same lighting conditions, and noticeably different if lighting changes.");
  calibrateSensor();
  delay(10000);
  calibrateSensor();
  Serial.println("The system will now test the light sensor's ability to record signals emitted by the flashlight.");
  Serial.println("Ensure the flashlight is aligned and facing the sensor directly for optimal signal capture.");
  delay(3000);

  
  xTaskCreatePinnedToCore(
    sendFlashesTask,// Function
    "SendFlashes",// Task name
    4096,// Stack size
    NULL,// Parameter
    1,// Priority
    NULL,// Handle
    0// Core 0
  );

  if (!testFlashDetectionAndCompare()){
    isTestPositive = false;
  }
  Serial.println("Now testing binary to text transformation");
  Serial.println("This will verify that binary Morse sequences are correctly decoded into readable text.");
  const char* expectedOutput = "TEST";
  const char* inputBinaryString ="1:600/0:600/1:200/0:605/1:220/0:185/1:200/0:200/1:200/0:600/1:600";
  Serial.print("Input : ");
  Serial.println(inputBinaryString);
  Serial.println("Expected output : TEST");
  char outputText[64];
  binaryToText(inputBinaryString,outputText);
  Serial.print("Output: ");
  Serial.println(outputText);
  if (strcmp(expectedOutput, outputText) != 0){
    isTestPositive = false;
    Serial.println("Failed to decode");
  }
  Serial.println("Now testing binary to text transformation (failure case)");
  const char* expectedOutputFail = "TEST";
  const char* inputBinaryStringFail = "1:200/0:200/1:200/0:200/1:200";   // Too short, does not match "TEST"
  Serial.print("Input : ");
  Serial.println(inputBinaryStringFail);
  Serial.print("Expected output : ");
  Serial.println(expectedOutputFail);

  char outputTextFail[64];
  binaryToText(inputBinaryStringFail, outputTextFail);

  Serial.print("Output: ");
  Serial.println(outputTextFail);

  if (strcmp(expectedOutputFail, outputTextFail) != 0) {
    Serial.println("Failed to decode (as expected).");
  } else {
    Serial.println("Unexpected success: decoding matched when it shouldn't have.");
        isTestPositive = false;
  }
  testHistoryFunctions();
  testMorseConversion();
  testWiFiConnection();
  testServerDiscovery();
  testMessageSendReceive();

  if (isTestPositive) {
    Serial.println("All tests passed!");
  } else {
    Serial.println("At least one test failed.");
  }
}

void loop(){}
