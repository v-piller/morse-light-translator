#include "M5Atom.h" // Pour les fonctionnalités M5Atom (M5.begin(), etc.)

// Bibliothèques personnalisées
#include "wifi_and_messages.h" // Gère la connexion Wi-Fi, mDNS et la communication HTTP
#include "string_to_light.h"   // Gère la transmission Morse via LED (utilise morse.h)
#include "morse.h"             // Gère la conversion texte <-> morse (utilisé par string_to_light)

// Constantes pour la connexion Wi-Fi
const char* ssid = "SURFACE-JEREMIE 2090";
const char* password = ">40Fc667";

// Variable globale pour stocker les détails du serveur découvert
struct ServerDetails morseServer;

// Variables pour la temporisation de l'envoi des messages
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 60000; // 60 secondes

void setup() {
  Serial.begin(115200);
  M5.begin(true, false, true); // Initialise M5Atom (Serial, I2C, Display activé)

  
  // Par sécurité,
  pinMode(FLASH_PIN, OUTPUT);      // Défini dans string_to_light.h comme G26
  digitalWrite(FLASH_PIN, LOW);    // S'assurer que la LED est éteinte au démarrage

  // Étape 1: Configuration et connexion Wi-Fi
  wifi_setup(ssid, password);

  // Étape 2: Recherche du serveur Morse via mDNS
  morseServer = server_setup();

  if (morseServer.found) {
    Serial.println("Setup terminé : Serveur Morse trouvé et prêt.");
    // Les détails (IP, port) sont déjà affichés par server_setup()
  } else {
    Serial.println("Setup terminé : Serveur Morse non trouvé.");
  }
}

void loop() {
  if (morseServer.found) {
    unsigned long currentTime = millis();

    // Envoi périodique d'un message de test
    if (currentTime - lastSendTime >= sendInterval) {
      Serial.println("Envoi d'un message de test périodique ('ping_from_device')...");
      sendMessage(morseServer, "ping_from_device"); // Utilise la fonction de wifi_and_messages
      lastSendTime = currentTime;
    }

    // Réception et traitement des messages
    String messageRecu = receiveMessage(morseServer); // Utilise la fonction de wifi_and_messages
    if (messageRecu.length() > 0) {
      Serial.print("Message reçu du serveur dans loop(): ");
      Serial.println(messageRecu);

      // Transmettre le message reçu en Morse via la LED
      // transmitMessage vient de string_to_light.h et utilise text_to_morse de morse.h
      transmitMessage(messageRecu.c_str());
    }

  } else {
    Serial.println("Serveur Morse non disponible. En attente...");
    delay(5000); // Attendre avant de potentiellement réessayer la découverte ou pour éviter le spam
    morseServer = server_setup();
    if(morseServer.found) Serial.println("Serveur Morse redécouvert !");
  }
  delay(100); // Petit délai général pour la boucle
}