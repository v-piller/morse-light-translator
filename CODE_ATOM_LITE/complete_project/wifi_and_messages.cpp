#include "wifi_and_messages.h"
#include "M5Atom.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <WiFiClient.h>



void wifi_setup(const char* ssid, const char* password){
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
    configTime(3600, 3600, "pool.ntp.org"); // fuseau horaire +1h, avec DST
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Erreur de synchronisation NTP");
    } else {
        Serial.println(&timeinfo, "Heure actuelle : %Y-%m-%d %H:%M:%S");
}

}

struct ServerDetails server_setup(){
    ServerDetails morseServer; // Déclarer morseServer au début de la fonction
    morseServer.found = false;
    // Initialise mDNS pour l'ESP32 (pour répondre si besoin, et pour chercher)
  if (!MDNS.begin("esp32-morse-client")) {
    Serial.println("Erreur configuration mDNS!");
    return morseServer;
  }
  Serial.println("Répondeur mDNS local démarré (esp32-morse-client.local)");
    delay(1000); // Laisser le temps au réseau et mDNS de s'initialiser
    Serial.println("Recherche du serveur Morse ('_http._tcp') via mDNS...");
    int8_t n = MDNS.queryService("http", "tcp"); // Cherche TOUS les services HTTP sur TCP
    if (n == 0) {
        Serial.println("Aucun service HTTP trouvé sur le réseau.");
    } 
    else {
        Serial.print(n);
        Serial.println(" service(s) HTTP trouvé(s):");
        for (int8_t i = 0; i < n; ++i) {
        // Affiche les détails de chaque service trouvé
        Serial.print("  "); Serial.print(i + 1); Serial.print(": ");
        Serial.print(MDNS.hostname(i)); // Nom de l'instance (ex: "morse-server")
        Serial.print(" ("); Serial.print(MDNS.IP(i));
        Serial.print(":"); Serial.print(MDNS.port(i)); Serial.println(")");

        // Vérifie si le nom de l'instance correspond à notre serveur
        String instanceName = MDNS.hostname(i);
        if (instanceName.equals("morse-server")) { // <-- DOIT correspondre à service_name_instance en Python
            Serial.println("      >>>> Serveur Morse trouvé! <<<<");
            morseServer.ip = MDNS.IP(i);    // Stocke l'IP
            morseServer.port = MDNS.port(i); // Stocke le Port
            morseServer.found = true;        // Met le flag à vrai
            break;
        }
        }
  }

    if (morseServer.found) {
        Serial.print("Adresse IP du serveur Morse : "); Serial.println(morseServer.ip);
        Serial.print("Port du serveur Morse : "); Serial.println(morseServer.port);
    } else {
        Serial.println("Serveur Morse ('morse-server._http._tcp.local.') non trouvé.");
    }
    return morseServer;
    }

// Fonction pour envoyer le message POST au serveur découvert
void sendMessage(struct ServerDetails serverDetails, const char* message) {
    // Vérifie si le serveur a été trouvé et si le WiFi est connecté
    if (!serverDetails.found || WiFi.status() != WL_CONNECTED) {
         Serial.println("Impossible d'envoyer le message: Serveur non trouvé ou WiFi déconnecté.");
         return;
    }

    WiFiClient client; // Crée un client WiFi
    HTTPClient http;   // Crée un client HTTP

    // Construit l'URL complète du endpoint /send
    String url = "http://" + serverDetails.ip.toString() + ":" + String(serverDetails.port) + "/send";
    Serial.print("Tentative d'envoi POST à: ");
    Serial.println(url);

    // Démarre la connexion HTTP
    if (http.begin(client, url)) {

        // Ajoute l'en-tête nécessaire pour les données de formulaire
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        // Prépare les données à envoyer (le message "test")
       String postData = "msg=" + String(message) + "&sender=device";
        // Exécute la requête POST
        int8_t httpCode = http.POST(postData);

        // Vérifie le code de retour HTTP
        if (httpCode > 0) {
            Serial.printf("Code de réponse HTTP: %d\n", httpCode);

            // Vérifie si la requête a réussi (Code 200 OK)
            if (httpCode == HTTP_CODE_OK) {
                 Serial.print("Message '");
                 Serial.print(message);
                 Serial.println("' envoyé avec succès au serveur!");
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

String receiveMessage(struct ServerDetails serverDetails) {
    static String lastMessage;
    if (!serverDetails.found || WiFi.status() != WL_CONNECTED) {
        Serial.println("[receiveMessage] Impossible de recevoir: Serveur non trouvé ou WiFi déconnecté.");
        return ""; // Retourne une chaîne vide
    }

    WiFiClient client;
    HTTPClient http;
    String messageContent = ""; // Pour stocker le message extrait
    String senderId = "";

    String url_str = "http://" + serverDetails.ip.toString() + ":" + String(serverDetails.port) + "/get";
    Serial.print("[receiveMessage] Tentative de réception GET depuis: ");
    Serial.println(url_str);

    if (http.begin(client, url_str.c_str())) {
        int httpCode = http.GET();

        if (httpCode > 0) {
            Serial.printf("[receiveMessage] Code de réponse HTTP GET: %d\n", httpCode);
            if (httpCode == HTTP_CODE_OK) {
                String payload = http.getString(); // Récupère la réponse JSON brute
                Serial.println("[receiveMessage] Réponse brute du serveur: " + payload);

                // Parsing JSON manuel amélioré pour extraire la valeur de "msg"
                int8_t key_idx = payload.indexOf("\"msg\""); // Cherche la clé exacte "\"msg\""
                if (key_idx != -1) {
                    // Clé trouvée. Chercher les deux-points après la clé.
                    // Longueur de "\"msg\"" est 5.
                    int8_t colon_idx = payload.indexOf(":", key_idx + 5);
                    if (colon_idx != -1) {
                        // Deux-points trouvés. Chercher le guillemet ouvrant de la valeur.
                        int8_t value_start_quote_idx = payload.indexOf("\"", colon_idx + 1);
                        if (value_start_quote_idx != -1) {
                            // Guillemet ouvrant trouvé. Chercher le guillemet fermant de la valeur.
                            int8_t value_end_quote_idx = payload.indexOf("\"", value_start_quote_idx + 1);
                            if (value_end_quote_idx != -1) {
                                // Guillemet fermant trouvé. Extraire la sous-chaîne.
                                messageContent = payload.substring(value_start_quote_idx + 1, value_end_quote_idx);
                                Serial.println("[receiveMessage] Message parsé ('msg'): " + messageContent);
                                // Extraire "sender"
                                int8_t sender_key_idx = payload.indexOf("\"sender\"");
                                if (sender_key_idx != -1) {
                                    int8_t sender_colon_idx = payload.indexOf(":", sender_key_idx + 8); 
                                    if (sender_colon_idx != -1) {
                                        int8_t sender_value_start_quote_idx = payload.indexOf("\"", sender_colon_idx + 1);
                                        if (sender_value_start_quote_idx != -1) {
                                            int8_t sender_value_end_quote_idx = payload.indexOf("\"", sender_value_start_quote_idx + 1);
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
                                    if (!messageContent.isEmpty() && messageContent == lastMessage){
                                        messageContent = "";
                                        Serial.println("[Message Canceled] already received");
                                    }
                                    else if (!messageContent.isEmpty()){
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
                    int8_t old_msg_key_start = payload.indexOf("\"message\"");
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
