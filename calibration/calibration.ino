#include <M5Atom.h>
#include <math.h>

// --- Constantes ---
const int calibration_duration_ms = 5000; // Durée de la phase de calibration en ms
const int sample_interval = 50;           // Intervalle entre les lectures du capteur en ms
const int buffer_size = calibration_duration_ms / sample_interval; // Nombre d'échantillons pour la calibration
const int max_entries = 100;              // Nombre maximum d'événements (ON/OFF) à stocker
const unsigned long maximum_idle_duration = 3000; // Durée d'inactivité maximale avant de considérer la séquence terminée en ms

// --- Variables globales pour la calibration et le seuil ---
uint16_t calibration_buffer[buffer_size]; // Buffer pour stocker les lectures pendant la calibration
float mean = 0;                           // Moyenne des lectures de calibration
float stddev = 0;                         // Écart-type des lectures de calibration
float median = 0;                         // Médiane des lectures de calibration
int min_value = 0;                        // Valeur minimale lue pendant la calibration
int threshold = 0;                        // Seuil de luminosité pour distinguer ON/OFF

// --- Variables globales pour la détection et le suivi d'état ---
bool idle = true;                         // Indique si le système est en état d'inactivité (pas de flash en cours)
unsigned long edge_start = 0;             // Moment (millis()) du dernier changement d'état détecté
unsigned long duration = 0;               // Durée de l'état précédent
unsigned long last_edge_time = 0;         // Moment (millis()) du dernier front détecté (utilisé pour l'inactivité)

// --- Structure pour stocker les événements (état + durée) ---
struct Etat {
    uint8_t etat;     // 0 = lumière OFF, 1 = lumière ON
    unsigned long duration; // durée en ms
};

// --- Tableau pour stocker la séquence d'événements détectés ---
Etat flash_table[max_entries];
int result_index = 0; // Index de la prochaine position libre dans flash_table

// --- Fonctions utilitaires pour les statistiques ---

// Fonction de comparaison pour qsort (tri pour la médiane)
int compare(const void *a, const void *b) {
    int val_a = *(uint16_t *)a;
    int val_b = *(uint16_t *)b;
    if (val_a < val_b) return -1;
    if (val_a > val_b) return 1;
    return 0;
}

// Calcule la médiane d'un tableau
float computeMedian(uint16_t arr[], int n) {
    qsort(arr, n, sizeof(uint16_t), compare);
    if (n % 2 == 0)
        return (arr[n / 2 - 1] + arr[n / 2]) / 2.0;
    else
        return arr[n / 2];
}

// Calcule la moyenne d'un tableau
float computeMean(uint16_t arr[], int n) {
    float sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum / n;
}

// Calcule l'écart-type d'un tableau
float computeStdDev(uint16_t arr[], int n, float mean) {
    float sumSq = 0;
    for (int i = 0; i < n; i++) {
        sumSq += pow(arr[i] - mean, 2);
    }
    return sqrt(sumSq / n);
}

// Trouve la valeur minimale dans un tableau
int findMin(uint16_t arr[], int n) {
    int minVal = arr[0];
    for (int i = 0; i < n; i++) {
        if (arr[i] < minVal) minVal = arr[i];
    }
    return minVal;
}

// Convertit le contenu de flash_table en une chaîne de caractères formatée
String flashTableToString() {
    String result = "";
    for (int i = 0; i < result_index; i++) {
        result += String(flash_table[i].etat) + ":" + String(flash_table[i].duration);
        if (i < result_index - 1) result += "/";
    }
    return result;
}

// --- Nouvelle fonction pour la calibration ---
void calibrateSensor() {
    Serial.println("Acquisition de données pour le calibrage...");
    unsigned long start_time = millis();
    int i = 0;

    // Collecte les échantillons de luminosité pendant la durée de calibration
    while (millis() - start_time < calibration_duration_ms && (i < buffer_size)) {
        calibration_buffer[i] = analogRead(32); // Lit la valeur du capteur (broche 32)
        delay(sample_interval);                 // Attend l'intervalle d'échantillonnage
        i++;
    }

    // Calcule les statistiques à partir des données de calibration
    mean = computeMean(calibration_buffer, buffer_size);
    stddev = computeStdDev(calibration_buffer, buffer_size, mean);
    median = computeMedian(calibration_buffer, buffer_size);
    min_value = findMin(calibration_buffer, buffer_size);

    // Calcule le seuil. Utilise la médiane + 1.5 * écart-type pour plus de robustesse.
    // Si une valeur basse = lumineux, alors une lecture < threshold = ON.
    threshold = median - 2.5 * stddev;

    // Affiche les résultats de la calibration
    Serial.printf("Calibration terminée. Mean: %.2f, StdDev: %.2f, Median: %.2f, Min: %d, Threshold: %d\n", mean, stddev, median, min_value, threshold);

    // Initialise le timer d'inactivité après la calibration
    last_edge_time = millis();
}

// --- Nouvelle fonction pour la détection des flashs et l'obtention des données ---
// Retourne une String contenant les données de la séquence si terminée, sinon une String vide.
String processFlashDetection() {
    // Vérifie si le tableau de résultats est plein
    if (result_index >= max_entries) {
        Serial.println("Tableau plein, séquence terminée.");
        String flashData = flashTableToString(); // Obtient la chaîne de données
        // Réinitialise les variables pour une nouvelle séquence
        result_index = 0;
        idle = true;
        edge_start = 0; // Réinitialise le début de l'arête
        last_edge_time = millis(); // Réinitialise le timer d'inactivité
        return flashData; // Retourne les données
    }

    uint16_t val = analogRead(32); // Lit la valeur actuelle du capteur
    unsigned long now = millis();  // Obtient le temps actuel

    // Détection du front montant (passage de OFF à ON)
    // Si le système est idle et la lecture est sous le seuil (lumineux)
    if (idle && val < threshold) {
        // Enregistre la durée de la pause (état OFF) qui vient de se terminer
        duration = now - edge_start;
        // N'enregistre pas la toute première "pause" avant le premier flash (quand edge_start est 0)
        if (edge_start > 0) {
            flash_table[result_index].etat = 0; // État OFF
            flash_table[result_index].duration = duration;
            result_index++;
            Serial.printf("OFF - durée : %lu ms\n", duration);
        }

        edge_start = now; // Marque le début de l'état ON
        idle = false;     // Le système n'est plus idle
        Serial.println("Flash ON détecté");
        last_edge_time = now; // Met à jour le timer d'inactivité
    }
    // Détection du front descendant (passage de ON à OFF)
    // Si le système n'est pas idle (est ON) et la lecture est au-dessus du seuil (sombre)
    else if (!idle && val > threshold) {
        // Enregistre la durée du flash (état ON) qui vient de se terminer
        duration = now - edge_start;
        flash_table[result_index].etat = 1; // État ON
        flash_table[result_index].duration = duration;
        result_index++;
        Serial.printf("ON - durée : %lu ms\n", duration);

        edge_start = now; // Marque le début de l'état OFF (pause)
        idle = true;      // Le système est maintenant idle
        Serial.println("Flash OFF détecté");
        last_edge_time = now; // Met à jour le timer d'inactivité
    }

    // Vérifie l'inactivité prolongée pour détecter la fin de la séquence
    if (idle && (millis() - last_edge_time > maximum_idle_duration)) {
        Serial.println("Transmission terminée (inactivité détectée).");
        String flashData = flashTableToString(); // Obtient la chaîne de données
        // Réinitialise les variables pour une nouvelle séquence
        result_index = 0;
        idle = true;
        edge_start = 0; // Réinitialise le début de l'arête
        last_edge_time = millis(); // Réinitialise le timer d'inactivité
        return flashData; // Retourne les données
    }

    delay(sample_interval); // Attend l'intervalle d'échantillonnage avant la prochaine lecture

    return ""; // Aucune séquence terminée, retourne une chaîne vide
}

// --- Fonction setup ---
void setup() {
    M5.begin(true, false, true); // Initialise le M5Atom (Serial, I2C, Power)
    Serial.begin(115200);        // Initialise la communication série
    pinMode(32, INPUT);          // Configure la broche 32 en entrée pour le capteur

    calibrateSensor(); // Appelle la fonction de calibration
}

// --- Fonction loop ---
void loop() {
    // Appelle la fonction de détection des flashs
    String detectedSequence = processFlashDetection();

    // Si la fonction a retourné une chaîne non vide, cela signifie qu'une séquence a été détectée
    if (detectedSequence.length() > 0) {
        Serial.println("--- Séquence de Flashs Détectée ---");
        Serial.println(detectedSequence); // Affiche la séquence détectée
        Serial.println("----------------------------------");
        // Le système est maintenant prêt à détecter la prochaine séquence
    }

    // La fonction processFlashDetection() contient déjà le delay(sample_interval);
    // donc pas besoin d'en ajouter un ici.
}
