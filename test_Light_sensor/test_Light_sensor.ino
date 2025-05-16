#include <M5Atom.h>
#include <math.h>

const int calibration_duration_ms = 5000;
const int sample_interval = 50;
const int buffer_size = calibration_duration_ms/sample_interval;

uint16_t calibration_buffer[buffer_size];

float mean = 0;
float stddev = 0;
int min_value = 0;
int threshold = 0;

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
    if (arr[i] < minVal) minVal = arr[i];
  }
  return minVal;
}

void setup() {
  M5.begin(true, false, true);
  Serial.begin(115200);
  pinMode(32, INPUT);
  // put your setup code here, to run once:

Serial.println("Acquisition de données pour le calibrage...");

unsigned long start_time = millis();
int i = 0;

while (millis() - start_time < calibration_duration_ms && (i < buffer_size)){
    calibration_buffer[i] = analogRead(32);
    delay(sample_interval);
    i++;
  }

Serial.println("Acquisition terminée.");
Serial.println("Export CSV :");

Serial.println("index,valeur");

mean = computeMean(calibration_buffer, buffer_size);
min_value = findMin(calibration_buffer, buffer_size);
stddev = computeStdDev(calibration_buffer, buffer_size, mean);

threshold = min_value - stddev; 
Serial.printf("Seuil de détection : %d\n", threshold);
}

void loop() {
  uint16_t val = analogRead(32);

  if (val < threshold) {
    Serial.println("1");
  } else {
    Serial.println("0");
  }

  delay(sample_interval);
}
