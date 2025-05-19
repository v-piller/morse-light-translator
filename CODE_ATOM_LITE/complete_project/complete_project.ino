#include "M5Atom.h" // Pour les fonctionnalités M5Atom (M5.begin(), etc.)

// Bibliothèques personnalisées
#include "wifi_and_messages.h" // Gère la connexion Wi-Fi, mDNS et la communication HTTP
#include "string_to_light.h"   // Gère la transmission Morse via LED (utilise morse.h)
#include "morse.h"             // Gère la conversion texte <-> morse (utilisé par string_to_light)
#include "binaryToText.h"
#include "flash_detector.h"

TaskHandle_t Task1 = NULL;
TaskHandle_t Task2 = NULL;

// Constantes pour la connexion Wi-Fi
const char* ssid = "SURFACE-JEREMIE 2090";
const char* password = ">40Fc667";

// Variable globale pour stocker les détails du serveur découvert
struct ServerDetails morseServer;

// Variables pour la temporisation de l'envoi des messages
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 60000; // 60 secondes
// output de la transformation de signaux à texte
char outputText[512];

void Task1code( void * pvParameters ) {
  Serial.print("Task 1 s'exécute sur le coeur : ");
  Serial.println(xPortGetCoreID()); // Affiche le coeur sur lequel cette tâche tourne

  // Boucle infinie de la tâche
  for(;;) {
    if (morseServer.found) {
      String detectedSequence = processFlashDetection();

  // Si la fonction a retourné une chaîne non vide, cela signifie qu'une séquence a été détectée
  if (detectedSequence.length() > 0) {
    Serial.println("--- Séquence de Flashs Détectée ---");
    Serial.println(detectedSequence); // Affiche la séquence détectée
    Serial.println("----------------------------------");
    binaryToText(detectedSequence.c_str(), outputText);
    Serial.print("Texte converti : ");
    Serial.println(outputText);
    sendMessage(morseServer, outputText);

  }

    } else {
    Serial.println("Serveur Morse non disponible. En attente...");
    delay(5000); // Attendre avant de potentiellement réessayer la découverte ou pour éviter le spam
    morseServer = server_setup();
    if(morseServer.found) Serial.println("Serveur Morse redécouvert !");
  }

    }
  }

void Task2code( void * pvParameters ) {
  Serial.print("Task 2 s'exécute sur le coeur : ");
  Serial.println(xPortGetCoreID()); // Affiche le coeur sur lequel cette tâche tourne

  // Boucle infinie de la tâche
  for(;;) {
    if (morseServer.found) {
      String messageRecu = receiveMessage(morseServer); // Utilise la fonction de wifi_and_messages
    if (messageRecu.length() > 0) {
      Serial.print("Message reçu du serveur dans loop(): ");
      Serial.println(messageRecu);

      // Transmettre le message reçu en Morse via la LED
      // transmitMessage vient de string_to_light.h et utilise text_to_morse de morse.h
      transmitMessage(messageRecu.c_str());
    }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

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
  pinMode(SENSOR_PIN, INPUT);  // Configure la broche du capteur en entrée (utilise la constante de la librairie)

  // Appelle la fonction de calibration de la librairie
  calibrateSensor();

  Serial.println("Prêt à détecter les séquences de flashs...");


  Serial.println("--- Démo Dual Core ESP32 ---");
  Serial.print("Setup() s'exécute sur le coeur : ");
  Serial.println(xPortGetCoreID()); // Affiche le coeur sur lequel setup() tourne (généralement 1)
  Serial.println("Création des tâches...");

  // Créer la Tâche 1 et l'épingler au Coeur 0
  xTaskCreatePinnedToCore(
              Task1code,      /* Fonction à implémenter pour la tâche */
              "Task1",        /* Nom de la tâche */
              10000,          /* Taille de la pile (stack size) en mots */
              NULL,           /* Paramètre d'entrée de la tâche (aucun ici) */
              1,              /* Priorité de la tâche (1 est faible) */
              &Task1,         /* Handle de la tâche (pour la gérer plus tard si besoin) */
              0);             /* Numéro du coeur (0) */

  delay(500); // Petite pause entre les créations de tâches

  // Créer la Tâche 2 et l'épingler au Coeur 1
  xTaskCreatePinnedToCore(
              Task2code,      /* Fonction à implémenter pour la tâche */
              "Task2",        /* Nom de la tâche */
              10000,          /* Taille de la pile */
              NULL,           /* Paramètre d'entrée */
              1,              /* Priorité */
              &Task2,         /* Handle */
              1);             /* Numéro du coeur (1) */

  Serial.println("Tâches créées.");
}

void loop() {
  delay(10);
}