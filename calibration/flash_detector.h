#ifndef FLASH_DETECTOR_H
#define FLASH_DETECTOR_H

#include <Arduino.h> // Nécessaire pour String, millis(), etc.
#include <M5Atom.h>  // Pour M5.begin() et pinMode() si tu veux les inclure dans le setup du sketch principal
#include <math.h>    // Pour sqrt() et pow()

// --- Constantes ---
// Elles peuvent rester globales pour être facilement accessibles
const int CALIBRATION_DURATION_MS = 5000; // Durée de la phase de calibration en ms
const int SAMPLE_INTERVAL = 50;           // Intervalle entre les lectures du capteur en ms
const int BUFFER_SIZE = CALIBRATION_DURATION_MS / SAMPLE_INTERVAL; // Nombre d'échantillons pour la calibration
const int MAX_ENTRIES = 100;              // Nombre maximum d'événements (ON/OFF) à stocker
const unsigned long MAXIMUM_IDLE_DURATION = 3000; // Durée d'inactivité maximale avant de considérer la séquence terminée en ms
const int SENSOR_PIN = 32;                // Broche du capteur de lumière

// --- Structure pour stocker les événements (état + durée) ---
// Définie ici pour être accessible si tu veux manipuler les données brutes dans ton sketch principal
struct FlashEvent {
    uint8_t state;     // 0 = lumière OFF, 1 = lumière ON
    unsigned long duration; // durée en ms
};

// --- Déclarations des variables globales ---
// Utilise 'extern' pour indiquer que ces variables sont définies ailleurs (dans le .cpp)
extern uint16_t calibration_buffer[BUFFER_SIZE];
extern float mean;
extern float stddev;
extern float median;
extern int min_value;
extern int threshold;

extern bool idle;
extern unsigned long edge_start;
extern unsigned long duration;
extern unsigned long last_edge_time;

extern FlashEvent flash_table[MAX_ENTRIES];
extern int result_index;

// --- Déclarations des fonctions publiques ---
// Ce sont les fonctions que tu appelleras depuis ton sketch principal
void calibrateSensor();

// Retourne une String contenant les données de la séquence si terminée, sinon une String vide.
String processFlashDetection();

// --- Déclarations des fonctions utilitaires (optionnel) ---
// Tu peux déclarer ici les fonctions utilitaires si tu souhaites les rendre accessibles
// depuis ton sketch principal, sinon, laisse-les uniquement dans le .cpp
// int compare(const void *a, const void *b); // Généralement pas nécessaire de rendre public
// float computeMedian(uint16_t arr[], int n);
// float computeMean(uint16_t arr[], int n);
// float computeStdDev(uint16_t arr[], int n, float mean);
// int findMin(uint16_t arr[], int n);
// String flashTableToString(); // Peut être utile si tu veux formater toi-même les données
// void resetDetectionState(); // Généralement pas nécessaire de rendre public

#endif
