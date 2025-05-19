#ifndef WIFI_AND_MESSAGES_H
#define WIFI_AND_MESSAGES_H

#include "M5Atom.h"
#include <WiFi.h>


/**
 * @brief Structure pour stocker les détails d'un serveur découvert sur le réseau.
 * * Contient l'adresse IP, le port et un indicateur booléen indiquant si le
 * serveur a été trouvé avec succès.
 */
struct ServerDetails {
  IPAddress ip;      // Adresse IP du serveur
  short port;          // Port du serveur
  bool found;        // true si le serveur a été trouvé, false sinon
};

/**
 * @brief Configure et initialise la connexion Wi-Fi de l'ESP32.
 * * Tente de se connecter au réseau Wi-Fi spécifié avec les identifiants fournis.
 * Affiche l'état de la connexion et l'adresse IP obtenue sur le port Série.
 * Bloque l'exécution en cas d'échec de connexion après plusieurs tentatives.
 * * @param ssid Le nom (SSID) du réseau Wi-Fi auquel se connecter.
 * @param password Le mot de passe du réseau Wi-Fi.
 */
void wifi_setup(const char* ssid, const char* password);

/**
 * @brief Recherche un serveur spécifique sur le réseau via mDNS.
 * * Initialise le service mDNS et recherche les services HTTP (_http._tcp).
 * Identifie un serveur spécifique par son nom d'instance (ex: "morse-server").
 * * @return ServerDetails Une structure contenant l'IP, le port et l'état de découverte
 * du serveur. 'found' sera true si le serveur est localisé.
 */
struct ServerDetails server_setup();

/**
 * @brief Envoie un message au serveur spécifié via une requête HTTP POST.
 * * Construit et envoie une requête POST au endpoint "/send" du serveur.
 * Le message est envoyé dans le corps de la requête sous forme de données de formulaire
 * (application/x-www-form-urlencoded) avec les clés "msg" et "sender".
 * Nécessite que les détails du serveur (IP, port) soient valides et que la connexion Wi-Fi soit active.
 * * @param serverDetails Structure contenant les informations du serveur (IP, port, état 'found').
 * @param message La chaîne de caractères à envoyer comme contenu du message.
 */
void sendMessage(struct ServerDetails serverDetails, const char* message);

/**
 * @brief Récupère un message depuis le serveur spécifié via une requête HTTP GET.
 * * Construit et envoie une requête GET au endpoint "/get" du serveur.
 * Tente de parser la réponse JSON attendue pour extraire le contenu du message ("msg")
 * et l'identifiant de l'expéditeur ("sender").
 * Gère les cas où le message provient de l'appareil lui-même ("device") ou a déjà été reçu.
 * Nécessite que les détails du serveur (IP, port) soient valides et que la connexion Wi-Fi soit active.
 * * @param serverDetails Structure contenant les informations du serveur (IP, port, état 'found').
 * @return String Le contenu du message reçu du serveur. Retourne une chaîne vide
 * en cas d'erreur, si aucun nouveau message n'est disponible, ou si le message
 * est filtré (auto-envoyé ou dupliqué).
 */
String receiveMessage(struct ServerDetails serverDetails);

#endif