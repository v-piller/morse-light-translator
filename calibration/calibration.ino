#include <M5Atom.h>
#include <math.h>


const int calibration_duration_ms = 5000;
const int sample_interval = 50;
const int buffer_size = calibration_duration_ms/sample_interval;
const int sample_interval_msg = 50;
const int max_entries = 100;



uint16_t calibration_buffer[buffer_size];

float mean = 0;
float stddev = 0;
float median = 0; 
int min_value = 0;
int threshold = 0;

bool idle = true; 
unsigned long edge_start = 0; 
unsigned long duration = 0;

struct Etat {
    uint8_t etat; // 0 = lumière OFF, 1 = lumière ON
    unsigned long duration; // durée en ms
  };

  Etat flash_table[max_entries];
  int result_index = 0;

int compare (const void *a, const void *b){
  int val_a = *(uint16_t *)a;
  int val_b = *(uint16_t *)b;

  if (val_a < val_b) return -1;
  if (val_a > val_b) return 1;
  return 0;
}

float computeMedian(uint16_t arr[], int n) {
  qsort(arr, n, sizeof(uint16_t), compare);
  if (n % 2 == 0)
    return (arr[n / 2 - 1] + arr[n / 2]) / 2.0;
  else
    return arr[n / 2];
}

float computeMean (uint16_t arr[], int n){
  float sum = 0;
  for (int i = 0; i < n; i++){
    sum += arr[i];
  }
  return sum / n;
}

float computeStdDev(uint16_t arr[], int n, float mean){
  float sumSq = 0;
  for(int i = 0; i < n; i++ ){
    sumSq += pow(arr[i] - mean, 2);
  }
  return sqrt(sumSq/n);
}

int findMin (uint16_t arr[], int n){
  int minVal = arr[0];
  for (int i = 0; i < n; i++){
    if (arr[i] > minVal) minVal = arr[i];
  }
  return minVal;
}

void setup() {
  M5.begin(true, false, true);
  Serial.begin(115200);
  pinMode(32, INPUT);


Serial.println("Acquisition de données pour le calibrage...");
unsigned long start_time = millis();
int i = 0;

while (millis() - start_time < calibration_duration_ms && (i < buffer_size)){
    calibration_buffer[i] = analogRead(32);
    delay(sample_interval);
    i++;
  }

mean = computeMean(calibration_buffer, buffer_size);
stddev = computeStdDev(calibration_buffer, buffer_size);
median = computeMedian(calibration_buffer, buffer_size);
min_value = findMin(calibration_buffer, buffer_size);

// Choisir une des deux stratégies :
// threshold = min_value + 0.5 * stddev;
threshold = median + 1.5 * stddev;
Serial.printf("Mean: %.2f, StdDev: %.2f, Median: %.2f, Min: %d, Threshold: %d\n", mean, stddev, median, min_value, threshold);


}

void loop() {
  if (result_index >= min_entries){
    Serial.println ("Tableau plein, arrêt de la détection");
    while (true);
    }
  uint16_t val = analogRead(32);
  unsigned long now = millis();

  if (idle && val < threshold){//sauf erreur plus c'est lumineux plus val est basse (à vérifier)
     // 🔺 Front montant (lumière vient de s’allumer)
     //on enregistre donc les données de la pause (idle moment qui vient de se terminer)
     duration = now - edge_start;
     //si edge start est encore à 0 c'est pas une pause mais l'attente avant le msg
     if (edge_start > 0){
      flash_table[result_index].etat = 0;
      flash_table[result_index].duration = duration;
      result_index++;
      Serial.printf("OFF - durée : %lu ms\n", duration);//%lu c'est le format specifier pour les unsigned long integers
     }

     edge_start = now; 
     idle = false;
     Serial.println("Flash ON");
  }
  else if (!idle && val > threshold){
  // 🔻 Front descendant (lumière vient de s’éteindre)
  duration = now - edge_start;// on retient la durée du flash
  flash_table[result_index].etat = 1; //ON
  flash_table[result_index].duration = duration;
  result_index++;
  Serial.printf("💡 ON - durée : %lu ms\n", duration);

  edge_start = now; 
  idle = true;
  Serial.println("Flash OFF");
  }

  delay(sample_interval);
  
  }
  
  

  





} 


