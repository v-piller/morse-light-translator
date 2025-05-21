#include "M5Atom.h"
#include <WiFi.h> //https://reference.arduino.cc/reference/en/libraries/wifi/(wifi.status/) => enlever les () pour la fonction, cut pour la librairie
#include <ESPmDNS.h>
#include <HTTPClient.h> //<-- requêtes http
#include <WiFiClient.h>

const char* ssid = "MATHILDE_LAPTOP 0057";//nom du réseau de mon ordi
const char* password = "2*04m9H2";//mot de passe du réseau

IPAddress serverIp; // pour stocker l'IP trouvée
int serverPort = 0; // Pour stocker le n° de port trouvé
bool serverFound = false; 




void setup() {
  Serial.begin(115200);// pour la communication du debug mitrocontroller avec l'ordi
  M5.begin(true, false, true);// Initialise M5Atom (Serial, I2C, Display)

  WiFi.mode(WIFI_STA); // Définit le mode Wi-Fi du microcontroller en Station
  Wifi.begin (ssid, password); //Tente la connexion du microcontroller au hostspot du PC
  Serial.println("Connexion au hotspot du PC...");

  int tries = 0; //compteur de tentatives
  while(WiFi.status() != WL_CONNECTED && tries < 20) { Timeout après ~10s
    delay(500);
    Serial.print(".");
    tries++;
  }

  if (WiFi.status() != WL_CONNECTED) {
      Serial.println(" Echec de connexion!");
      // Bloquer ou redémarrer ?
      while(true) delay(1000);
  }

  Serial.println("Connecté!");
  Serial.print("Adresse IP de l'ESP32 : ");
  Serial.println(WiFi.localIP()); // Affiche l'IP obtenue




  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
