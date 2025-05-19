#include <M5Atom.h>
#include "flash_detector.h" // Inclure le fichier d'en-tête de ta librairie

void setup() {
  M5.begin(true, false, true); // Initialise le M5Atom (Serial, I2C, Power)
  Serial.begin(115200);        // Initialise la communication série
  pinMode(SENSOR_PIN, INPUT);  // Configure la broche du capteur en entrée (utilise la constante de la librairie)

  // Appelle la fonction de calibration de la librairie
  calibrateSensor();

  Serial.println("Prêt à détecter les séquences de flashs...");
}

void loop() {
  // Appelle la fonction de détection des flashs de la librairie
  String detectedSequence = processFlashDetection();

  // Si la fonction a retourné une chaîne non vide, cela signifie qu'une séquence a été détectée
  if (detectedSequence.length() > 0) {
    Serial.println("--- Séquence de Flashs Détectée ---");
    Serial.println(detectedSequence); // Affiche la séquence détectée (sans les unités)
    Serial.println("----------------------------------");
    // Ici, tu peux ajouter du code pour traiter la chaîne detectedSequence
    // par exemple, la décoder en message, envoyer via Bluetooth, etc.
  }

  // La fonction processFlashDetection() gère déjà le délai entre les lectures.
}
