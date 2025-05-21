#include "flash_detector.h"

// --- Définitions des variables globales ---
// Elles sont définies ici, sans le mot-clé 'extern'
uint16_t calibration_buffer[BUFFER_SIZE];
float mean = 0;
float stddev = 0;
float median = 0;
int min_value = 0;
int threshold = 0;

bool idle = true;
unsigned long edge_start = 0;
unsigned long duration = 0;
unsigned long last_edge_time = 0;

FlashEvent flash_table[MAX_ENTRIES];
int result_index = 0;

// --- Implémentations des fonctions utilitaires (privées à ce fichier) ---
// Elles ne sont pas déclarées dans le .h si tu ne veux pas qu'elles soient publiques

// Fonction de comparaison pour qsort (tri pour la médiane)
static int compare(const void *a, const void *b) { // 'static' limite la portée à ce fichier
    int val_a = *(uint16_t *)a;
    int val_b = *(uint16_t *)b;
    if (val_a < val_b) return -1;
    if (val_a > val_b) return 1;
    return 0;
}

// Calcule la médiane d'un tableau
static float computeMedian(uint16_t arr[], int n) {
    qsort(arr, n, sizeof(uint16_t), compare);
    if (n % 2 == 0)
        return (arr[n / 2 - 1] + arr[n / 2]) / 2.0;
    else
        return arr[n / 2];
}

// Calcule la moyenne d'un tableau
static float computeMean(uint16_t arr[], int n) {
    float sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum / n;
}

// Calcule l'écart-type d'un tableau
static float computeStdDev(uint16_t arr[], int n, float mean) {
    float sumSq = 0;
    for (int i = 0; i < n; i++) {
        sumSq += pow(arr[i] - mean, 2);
    }
    return sqrt(sumSq / n);
}

// Trouve la valeur minimale dans un tableau
static int findMin(uint16_t arr[], int n) {
    int minVal = arr[0];
    for (int i = 0; i < n; i++) {
        if (arr[i] < minVal) minVal = arr[i];
    }
    return minVal;
}

// Convertit le contenu de flash_table en une chaîne de caractères formatée (sans les unités "ms")
static String flashTableToString() {
    String result = "";
    for (int i = 0; i < result_index; i++) {
        result += String(flash_table[i].state) + ":" + String(flash_table[i].duration);
        if (i < result_index - 1) result += "/";
    }
    return result;
}

// Réinitialise l'état interne pour commencer une nouvelle détection
static void resetDetectionState() {
    result_index = 0;
    idle = true;
    edge_start = 0;
    last_edge_time = millis(); // Réinitialise le timer d'inactivité
}

// --- Implémentation de la fonction publique de calibration ---
void calibrateSensor() {
    Serial.println("Acquisition de données pour le calibrage...");
    unsigned long start_time = millis();
    int i = 0;

    // Collecte les échantillons de luminosité pendant la durée de calibration
    while (millis() - start_time < CALIBRATION_DURATION_MS && (i < BUFFER_SIZE)) {
        calibration_buffer[i] = analogRead(SENSOR_PIN); // Lit la valeur du capteur
        delay(SAMPLE_INTERVAL);                 // Attend l'intervalle d'échantillonnage
        i++;
    }

    // Calcule les statistiques à partir des données de calibration
    mean = computeMean(calibration_buffer, BUFFER_SIZE);
    stddev = computeStdDev(calibration_buffer, BUFFER_SIZE, mean);
    median = computeMedian(calibration_buffer, BUFFER_SIZE);
    min_value = findMin(calibration_buffer, BUFFER_SIZE);

    // Calcule le seuil en utilisant la stratégie min_value - 5 * stddev
    float calculated_threshold = min_value - 5.0 * stddev; // Utilise 5.0 pour la multiplication flottante

    // S'assure que le seuil n'est pas négatif
    if (calculated_threshold < 0) {
        threshold = 0;
    } else {
        threshold = (int)calculated_threshold; // Convertit en entier pour le seuil
    }

    // Affiche les résultats de la calibration
    Serial.printf("Calibration terminée. Mean: %.2f, StdDev: %.2f, Median: %.2f, Min: %d, Threshold: %d\n", mean, stddev, median, min_value, threshold);

    // Initialise le timer d'inactivité après la calibration
    last_edge_time = millis();
}

// --- Implémentation de la fonction publique de détection des flashs ---
String processFlashDetection() {
    // Vérifie si le tableau de résultats est plein
    if (result_index >= MAX_ENTRIES) {
        Serial.println("Tableau plein, séquence terminée.");
        String flashData = flashTableToString(); // Obtient la chaîne de données
        resetDetectionState(); // Réinitialise pour une nouvelle séquence
        return flashData; // Retourne les données
    }

    uint16_t val = analogRead(SENSOR_PIN); // Lit la valeur actuelle du capteur
    unsigned long now = millis();  // Obtient le temps actuel

    // Détection du front montant (passage de OFF à ON)
    // Si le système est idle et la lecture est sous le seuil (valeur basse = lumineux)
    if (idle && val < threshold) {
        // Enregistre la durée de la pause (état OFF) qui vient de se terminer
        duration = now - edge_start;
        // N'enregistre pas la toute première "pause" avant le premier flash (quand edge_start est 0)
        if (edge_start > 0) {
            flash_table[result_index].state = 0; // État OFF
            flash_table[result_index].duration = duration;
            result_index++;
            Serial.printf("OFF - durée : %lu ms\n", duration); // L'unité "ms" est conservée pour l'affichage série
        }

        edge_start = now; // Marque le début de l'état ON
        idle = false;     // Le système n'est plus idle
        Serial.println("Flash ON détecté");
        last_edge_time = now; // Met à jour le timer d'inactivité
    }
    // Détection du front descendant (passage de ON à OFF)
    // Si le système n'est pas idle (est ON) et la lecture est au-dessus du seuil (valeur haute = sombre)
    else if (!idle && val > threshold) {
        // Enregistre la durée du flash (état ON) qui vient de se terminer
        duration = now - edge_start;
        flash_table[result_index].state = 1; // État ON
        flash_table[result_index].duration = duration;
        result_index++;
        Serial.printf("ON - durée : %lu ms\n", duration); // L'unité "ms" est conservée pour l'affichage série

        edge_start = now; // Marque le début de l'état OFF (pause)
        idle = true;      // Le système est maintenant idle
        Serial.println("Flash OFF détecté");
        last_edge_time = now; // Met à jour le timer d'inactivité
    }

    // Vérifie l'inactivité prolongée pour détecter la fin de la séquence
    if (idle && (millis() - last_edge_time > MAXIMUM_IDLE_DURATION)) {
        Serial.println("Transmission terminée (inactivité détectée).");
        String flashData = flashTableToString(); // Obtient la chaîne de données (sans les unités)
        resetDetectionState(); // Réinitialise pour une nouvelle séquence
        return flashData; // Retourne les données
    }

    delay(SAMPLE_INTERVAL); // Attend l'intervalle d'échantillonnage avant la prochaine lecture

    return ""; // Aucune séquence terminée, retourne une chaîne vide
}
