#include "M5Atom.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <HTTPClient.h> // <-- Pour faire des requêtes HTTP
#include <WiFiClient.h>
#include "string_to_light.h"
#include "morse.h"

const char* ssid = "SURFACE-JEREMIE 2090"; // Nom du hotspot créé sur le PC
const char* password = ">40Fc667"; // Mot de passe du hotspot

IPAddress serverIp; // Pour stocker l'IP trouvée
int serverPort = 0; // Pour stocker le port trouvé
bool serverFound = false; // Flag pour indiquer si le serveur a été trouvé

void setup() {
  Serial.begin(115200);
  M5.begin(true, false, true); // Initialise M5Atom (Serial, I2C, Display)
  pinMode(FLASH_PIN, OUTPUT);
  digitalWrite(FLASH_PIN, LOW);
  WiFi.mode(WIFI_STA); // Définit le mode Wi-Fi en Station
  WiFi.begin(ssid, password); // Tente la connexion au hotspot du PC
  Serial.print("Connexion au hotspot du PC...");

  int tries = 0; // Compteur de tentatives
  while (WiFi.status() != WL_CONNECTED && tries < 20) { // Timeout après ~10s
    delay(500);
    Serial.print(".");
    tries++;
  }

  if (WiFi.status() != WL_CONNECTED) {
      Serial.println(" Echec de connexion!");
      // Bloquer ou redémarrer ?
      while(true) delay(1000);
  }

  Serial.println(" Connecté!");
  Serial.print("Adresse IP de l'ESP32 : ");
  Serial.println(WiFi.localIP()); // Affiche l'IP obtenue

  // Initialise mDNS pour l'ESP32 (pour répondre si besoin, et pour chercher)
  if (!MDNS.begin("esp32-morse-client")) {
    Serial.println("Erreur configuration mDNS!");
    return; // Ou gérer l'erreur autrement
  }
  Serial.println("Répondeur mDNS local démarré (esp32-morse-client.local)");

  // Optionnel : Annoncer un service propre à l'ESP si nécessaire
  // MDNS.addService("esp-webserver", "tcp", 80);

  delay(1000); // Laisser le temps au réseau et mDNS de s'initialiser

  Serial.println("Recherche du serveur Morse ('_http._tcp') via mDNS...");

  int n = MDNS.queryService("http", "tcp"); // Cherche TOUS les services HTTP sur TCP
  if (n == 0) {
    Serial.println("Aucun service HTTP trouvé sur le réseau.");
  } else {
    Serial.print(n);
    Serial.println(" service(s) HTTP trouvé(s):");
    for (int i = 0; i < n; ++i) {
      // Affiche les détails de chaque service trouvé
      Serial.print("  "); Serial.print(i + 1); Serial.print(": ");
      Serial.print(MDNS.hostname(i)); // Nom de l'instance (ex: "morse-server")
      Serial.print(" ("); Serial.print(MDNS.IP(i));
      Serial.print(":"); Serial.print(MDNS.port(i)); Serial.println(")");

      // Vérifie si le nom de l'instance correspond à notre serveur
      String instanceName = MDNS.hostname(i);
      if (instanceName.equals("morse-server")) { // <-- DOIT correspondre à service_name_instance en Python
         Serial.println("      >>>> Serveur Morse trouvé! <<<<");
         serverIp = MDNS.IP(i);    // Stocke l'IP
         serverPort = MDNS.port(i); // Stocke le Port
         serverFound = true;        // Met le flag à vrai
         // On pourrait sortir de la boucle si le premier trouvé nous suffit:
         // break;
      }
    }
  }

  if (serverFound) {
      Serial.print("Adresse IP du serveur Morse : "); Serial.println(serverIp);
      Serial.print("Port du serveur Morse : "); Serial.println(serverPort);
      // C'est ici que vous pouvez utiliser serverIp et serverPort pour communiquer
      // avec votre serveur Flask (par ex. avec un client HTTP)
  } else {
      Serial.println("Serveur Morse ('morse-server._http._tcp.local.') non trouvé.");
      // Mettre en place une logique de secours ou afficher une erreur sur l'ESP
  }
}
// ... (après vos variables globales ssid, password, serverIp, serverPort, serverFound) ...

// Fonction pour envoyer le message POST au serveur découvert
void sendTestMessage() {
    // Vérifie si le serveur a été trouvé et si le WiFi est connecté
    if (!serverFound || WiFi.status() != WL_CONNECTED) {
         Serial.println("Impossible d'envoyer le message: Serveur non trouvé ou WiFi déconnecté.");
         return;
    }

    WiFiClient client; // Crée un client WiFi
    HTTPClient http;   // Crée un client HTTP

    // Construit l'URL complète du endpoint /send
    String url = "http://" + serverIp.toString() + ":" + String(serverPort) + "/send";
    Serial.print("Tentative d'envoi POST à: ");
    Serial.println(url);

    // Démarre la connexion HTTP
    // Note: La syntaxe peut légèrement varier selon la version du core ESP32
    // http.begin(url); // Pour les anciennes versions
    if (http.begin(client, url)) { // Préféré pour les versions plus récentes

        // Ajoute l'en-tête nécessaire pour les données de formulaire
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        // Prépare les données à envoyer (le message "test")
       String postData = "msg=test&sender=device";
       http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        // Exécute la requête POST
        int httpCode = http.POST(postData);

        // Vérifie le code de retour HTTP
        if (httpCode > 0) {
            Serial.printf("Code de réponse HTTP: %d\n", httpCode);

            // Vérifie si la requête a réussi (Code 200 OK)
            if (httpCode == HTTP_CODE_OK) {
                 Serial.println("Message 'test' envoyé avec succès au serveur!");
                 // Vous pourriez lire la réponse si nécessaire:
                 // String payload = http.getString();
                 // Serial.println("Réponse: " + payload);
            } else {
                 Serial.println("Le serveur a répondu avec une erreur.");
            }
        } else {
            // Erreur lors de la connexion ou de l'envoi
            Serial.printf("Echec de l'envoi POST, erreur: %s\n", http.errorToString(httpCode).c_str());
        }

        // Termine la connexion HTTP (important!)
        http.end();

    } else {
        Serial.println("Echec de l'initialisation de la connexion HTTP.");
    }
}
// Fonction modifiée pour recevoir ET PARSER le message du serveur
String receiveMessage() {
    static String lastMessage;
    if (!serverFound || WiFi.status() != WL_CONNECTED) {
        Serial.println("[receiveMessage] Impossible de recevoir: Serveur non trouvé ou WiFi déconnecté.");
        return ""; // Retourne une chaîne vide
    }

    WiFiClient client;
    HTTPClient http;
    String messageContent = ""; // Pour stocker le message extrait
    String senderId = "";

    String url_str = "http://" + serverIp.toString() + ":" + String(serverPort) + "/get";
    Serial.print("[receiveMessage] Tentative de réception GET depuis: ");
    Serial.println(url_str);

    if (http.begin(client, url_str.c_str())) { // Utiliser .c_str()
        int httpCode = http.GET();

        if (httpCode > 0) {
            Serial.printf("[receiveMessage] Code de réponse HTTP GET: %d\n", httpCode);
            if (httpCode == HTTP_CODE_OK) {
                String payload = http.getString(); // Récupère la réponse JSON brute
                Serial.println("[receiveMessage] Réponse brute du serveur: " + payload);

                // Parsing JSON manuel amélioré pour extraire la valeur de "msg"
                int key_idx = payload.indexOf("\"msg\""); // Cherche la clé exacte "\"msg\""
                if (key_idx != -1) {
                    // Clé trouvée. Chercher les deux-points après la clé.
                    // Longueur de "\"msg\"" est 5.
                    int colon_idx = payload.indexOf(":", key_idx + 5);
                    if (colon_idx != -1) {
                        // Deux-points trouvés. Chercher le guillemet ouvrant de la valeur.
                        int value_start_quote_idx = payload.indexOf("\"", colon_idx + 1);
                        if (value_start_quote_idx != -1) {
                            // Guillemet ouvrant trouvé. Chercher le guillemet fermant de la valeur.
                            int value_end_quote_idx = payload.indexOf("\"", value_start_quote_idx + 1);
                            if (value_end_quote_idx != -1) {
                                // Guillemet fermant trouvé. Extraire la sous-chaîne.
                                messageContent = payload.substring(value_start_quote_idx + 1, value_end_quote_idx);
                                Serial.println("[receiveMessage] Message parsé ('msg'): " + messageContent);
                                // Extraire "sender"
                                int sender_key_idx = payload.indexOf("\"sender\"");
                                if (sender_key_idx != -1) {
                                    int sender_colon_idx = payload.indexOf(":", sender_key_idx + 8); 
                                    if (sender_colon_idx != -1) {
                                        int sender_value_start_quote_idx = payload.indexOf("\"", sender_colon_idx + 1);
                                        if (sender_value_start_quote_idx != -1) {
                                            int sender_value_end_quote_idx = payload.indexOf("\"", sender_value_start_quote_idx + 1);
                                            if (sender_value_end_quote_idx != -1) {
                                                // C'EST ICI QUE senderId OBTIENT SA VALEUR :
                                                senderId = payload.substring(sender_value_start_quote_idx + 1, sender_value_end_quote_idx);
                                                Serial.println("[receiveMessage] Expéditeur parsé ('sender'): " + senderId);
                                            }
                                        }
                                    }
                                } else {
                                    Serial.println("[receiveMessage] Clé '\"sender\"' non trouvée dans JSON.");
                                }
                                if (senderId.equals("device")) {
                                    messageContent = "";
                                    Serial.println("[Message Canceled] message comming from self");
                                    }
                                else{
                                    if (messageContent == lastMessage){
                                        messageContent = "";
                                        Serial.println("[Message Canceled] already received");
                                    }
                                    else{
                                        lastMessage = messageContent;
                                    }
                                }
                            } else {
                                Serial.println("[receiveMessage] Erreur parsing JSON: Guillemet de fin de valeur pour 'msg' non trouvé.");
                            }
                        } else {
                            Serial.println("[receiveMessage] Erreur parsing JSON: Guillemet de début de valeur pour 'msg' non trouvé.");
                        }
                    } else {
                        Serial.println("[receiveMessage] Erreur parsing JSON: Deux-points après la clé 'msg' non trouvés.");
                    }
                } else {
                    Serial.println("[receiveMessage] Erreur parsing JSON: Clé '\"msg\"' non trouvée.");
                    // Optionnel: Tenter de parser "message" comme fallback ou pour débogage
                    // si votre serveur pouvait utiliser l'une ou l'autre clé.
                    int old_msg_key_start = payload.indexOf("\"message\"");
                    if (old_msg_key_start != -1) {
                         Serial.println("[receiveMessage] Info: Ancienne clé '\"message\"' trouvée, mais '\"msg\"' était attendue et non trouvée.");
                    }
                }
            } else {
                Serial.printf("[receiveMessage] Le serveur a répondu avec une erreur (GET): %d\n", httpCode);
            }
        } else {
            Serial.printf("[receiveMessage] Echec de la requête GET, erreur: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();
    } else {
        Serial.println("[receiveMessage] Echec de l'initialisation de la connexion HTTP (GET).");
    }
    return messageContent; // Retourne le message extrait ou une chaîne vide
}
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 60000;
void loop() {
    unsigned long counter = 0;
  // Votre code principal ici
  // Par exemple, envoyer des requêtes au serveur si serverFound est true
  if (serverFound) {
    unsigned long now = millis();
    if (now - lastSendTime >= sendInterval) {
      sendTestMessage();
      lastSendTime = now;
    }

    transmitMessage(receiveMessage().c_str());
  } else {
      // Indiquer l'erreur (ex: LED clignotante)
  }
}