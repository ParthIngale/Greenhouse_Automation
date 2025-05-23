// Mere Dil se edit kiya hua code
// Fully edited working only firebase part
// All 7 Readings

#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "DHT.h"
#include <time.h>

// === Wi-Fi Credentials ===
#define WIFI_SSID "Opple"
#define WIFI_PASSWORD "123456789p"

// === Firebase Credentials ===
#define API_KEY "AIzaSyDtltj1zgi0WheP8k48nn8L_O0ISDQjbVE"
#define DATABASE_URL "https://final-63d5f-default-rtdb.asia-southeast1.firebasedatabase.app/"

// === DHT Sensor ===
#define DHTPIN 25
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// === Sensor Pins ===
#define SOIL_MOISTURE_PIN 32
#define GAS_SENSOR_PIN 34
#define WATER_SENSOR_PIN 35

// === Thresholds ===
#define TEMP_THRESHOLD 30.4
#define SOIL_MOISTURE_THRESHOLD 50 // Changed to percentage
#define GAS_SENSOR_THRESHOLD 500   // Still available if needed

// === L298N Motor Driver Pins ===
#define PUMP_IN1 4
#define PUMP_IN2 5
#define FAN_IN3 26
#define FAN_IN4 27

String fanStatus = "OFF";
String pumpStatus = "OFF";

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void connectWiFi() {
  WiFi.disconnect(true);
  delay(1000);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\n Wi-Fi Connected!");
}

void syncTime() {
  const long gmtOffset_sec = 19800;
  const int daylightOffset_sec = 0;
  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "time.nist.gov");

  Serial.print("Syncing time");
  time_t now = time(nullptr);
  while (now < 1000000000) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("\n Time synced");
}
void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(GAS_SENSOR_PIN, INPUT);
  pinMode(SOIL_MOISTURE_PIN, INPUT);
  pinMode(WATER_SENSOR_PIN, INPUT);

  pinMode(PUMP_IN1, OUTPUT);
  pinMode(PUMP_IN2, OUTPUT);
  pinMode(FAN_IN3, OUTPUT);
  pinMode(FAN_IN4, OUTPUT);

  digitalWrite(PUMP_IN1, LOW);
  digitalWrite(PUMP_IN2, LOW);
  digitalWrite(FAN_IN3, LOW);
  digitalWrite(FAN_IN4, LOW);

  Serial.println("System Initialized");

  connectWiFi();
  syncTime();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase Anonymous Auth Success");
  } else {
    Serial.printf("Firebase Auth failed: %s\n", config.signer.signupError.message.c_str());
  }
}

void loop() {
  delay(10000);

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  int rawSoil = analogRead(SOIL_MOISTURE_PIN);
  int soilMoisture = map(rawSoil, 3000, 0, 0, 100); // Convert to %

  int gasLevel = analogRead(GAS_SENSOR_PIN);
  int waterLevel = digitalRead(WATER_SENSOR_PIN);

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
  }

  // === Fan Logic ===
  if (temperature > TEMP_THRESHOLD) {
    digitalWrite(FAN_IN3, HIGH);
    digitalWrite(FAN_IN4, LOW);
    fanStatus = "ON";
  } else {
    digitalWrite(FAN_IN3, LOW);
    digitalWrite(FAN_IN4, LOW);
    fanStatus = "OFF";
  }

  // === Pump Logic ===
  if (soilMoisture < SOIL_MOISTURE_THRESHOLD && waterLevel == HIGH) {
    digitalWrite(PUMP_IN1, HIGH);
    digitalWrite(PUMP_IN2, LOW);
    pumpStatus = "ON";
  } else {
    digitalWrite(PUMP_IN1, LOW);
    digitalWrite(PUMP_IN2, LOW);
    pumpStatus = "OFF";
  }

  String waterStatus = (waterLevel == HIGH) ? "Water Detected" : "Dry";

  // === Serial Output ===
  Serial.println("\n=========Greenhouse Status =========");
  Serial.printf("Temperature: %.2f °C\n", temperature);
  Serial.printf("Humidity: %.2f %%\n", humidity);
  Serial.printf("Fan Status: %s\n", fanStatus.c_str());
  Serial.printf("Pump Status: %s\n", pumpStatus.c_str());
  Serial.printf("Gas Sensor: %d ppm\n", gasLevel);
  Serial.printf("Water Sensor: %s\n", waterStatus.c_str());
  Serial.printf("Soil Moisture: %d %%\n", soilMoisture);
  Serial.printf("Soil Moisture value: %d \n", rawSoil);
  

  // === Firebase Upload ===
  if (Firebase.ready()) {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    char timestamp[30];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);

    FirebaseJson json;

    char tempBuffer[10];
    char humBuffer[10];
    char gasBuffer[20];
    char soilBuffer[10];

    snprintf(tempBuffer, sizeof(tempBuffer), "%.2f °C", temperature);
    snprintf(humBuffer, sizeof(humBuffer), "%.2f %%", humidity);
    snprintf(gasBuffer, sizeof(gasBuffer), "%d ppm", gasLevel);
    snprintf(soilBuffer, sizeof(soilBuffer), "%d %%", soilMoisture);

    json.set("temperature", String(tempBuffer));
    json.set("humidity", String(humBuffer));
    json.set("fanStatus", fanStatus);
    json.set("pumpStatus", pumpStatus);
    json.set("gasLevel(ppm)", String(gasBuffer));
    json.set("waterStatus", waterStatus);
    json.set("soilMoisture", String(soilBuffer));
    json.set("timestamp", String(timestamp));

    if (Firebase.RTDB.pushJSON(&fbdo, "/Greenhouse", &json)) {
      Serial.println("Data sent to Firebase!");
    } else {
      Serial.printf("Firebase push failed: %s\n", fbdo.errorReason().c_str());
    }
  } else {
    Serial.println("Firebase not ready");
  }
}


// #include <WiFi.h>
// #include <Firebase_ESP_Client.h>
// #include "DHT.h"
// #include <time.h>

// // === Wi-Fi Credentials ===
// #define WIFI_SSID "Opple"
// #define WIFI_PASSWORD "123456789p"

// // === Firebase Credentials ===
// #define API_KEY "AIzaSyDtltj1zgi0WheP8k48nn8L_O0ISDQjbVE"
// #define DATABASE_URL "https://final-63d5f-default-rtdb.asia-southeast1.firebasedatabase.app/"

// // === DHT Sensor ===
// #define DHTPIN 25
// #define DHTTYPE DHT11
// DHT dht(DHTPIN, DHTTYPE);

// // === Sensor Pins ===
// #define SOIL_MOISTURE_PIN 32
// #define GAS_SENSOR_PIN 34
// #define WATER_SENSOR_PIN 35

// // === Thresholds ===
// #define TEMP_THRESHOLD 36.4
// #define SOIL_MOISTURE_THRESHOLD 50 // Changed to percentage
// #define GAS_SENSOR_THRESHOLD 500   // Still available if needed

// // === L298N Motor Driver Pins ===
// #define PUMP_IN1 4
// #define PUMP_IN2 5
// #define FAN_IN3 26
// #define FAN_IN4 27

// String fanStatus = "OFF";
// String pumpStatus = "OFF";

// FirebaseData fbdo;
// FirebaseAuth auth;
// FirebaseConfig config;

// void connectWiFi() {
//   WiFi.disconnect(true);
//   delay(1000);

//   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//   Serial.print("Connecting to Wi-Fi");

//   while (WiFi.status() != WL_CONNECTED) {
//     Serial.print(".");
//     delay(500);
//   }

//   Serial.println("\n Wi-Fi Connected!");
// }

// void syncTime() {
//   const long gmtOffset_sec = 19800;
//   const int daylightOffset_sec = 0;
//   configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "time.nist.gov");

//   Serial.print("Syncing time");
//   time_t now = time(nullptr);
//   while (now < 1000000000) {
//     delay(500);
//     Serial.print(".");
//     now = time(nullptr);
//   }
//   Serial.println("\n Time synced");
// }

// void setup() {
//   Serial.begin(115200);
//   dht.begin();

//   pinMode(GAS_SENSOR_PIN, INPUT);
//   pinMode(SOIL_MOISTURE_PIN, INPUT);
//   pinMode(WATER_SENSOR_PIN, INPUT);

//   pinMode(PUMP_IN1, OUTPUT);
//   pinMode(PUMP_IN2, OUTPUT);
//   pinMode(FAN_IN3, OUTPUT);
//   pinMode(FAN_IN4, OUTPUT);

//   digitalWrite(PUMP_IN1, LOW);
//   digitalWrite(PUMP_IN2, LOW);
//   digitalWrite(FAN_IN3, LOW);
//   digitalWrite(FAN_IN4, LOW);

//   Serial.println("System Initialized");

//   connectWiFi();
//   syncTime();

//   config.api_key = API_KEY;
//   config.database_url = DATABASE_URL;
//   Firebase.begin(&config, &auth);
//   Firebase.reconnectWiFi(true);

//   if (Firebase.signUp(&config, &auth, "", "")) {
//     Serial.println("Firebase Anonymous Auth Success");
//   } else {
//     Serial.printf("Firebase Auth failed: %s\n", config.signer.signupError.message.c_str());
//   }
// }

// void loop() {
//   delay(10000);

//   float humidity = dht.readHumidity();
//   float temperature = dht.readTemperature();

//   int rawSoil = analogRead(SOIL_MOISTURE_PIN);
//   int soilMoisture = map(rawSoil, 4095, 0, 0, 100); // Convert to %

//   int gasLevel = analogRead(GAS_SENSOR_PIN);
//   int waterLevel = digitalRead(WATER_SENSOR_PIN);

//   if (isnan(humidity) || isnan(temperature)) {
//     Serial.println("Failed to read from DHT sensor!");
//   }

//   // === Fan Logic ===
//   if (temperature > TEMP_THRESHOLD) {
//     digitalWrite(FAN_IN3, HIGH);
//     digitalWrite(FAN_IN4, LOW);
//     fanStatus = "ON";
//   } else {
//     digitalWrite(FAN_IN3, LOW);
//     digitalWrite(FAN_IN4, LOW);
//     fanStatus = "OFF";
//   }

//   // === Pump Logic ===
//   if (soilMoisture < SOIL_MOISTURE_THRESHOLD && waterLevel == HIGH) {
//     digitalWrite(PUMP_IN1, HIGH);
//     digitalWrite(PUMP_IN2, LOW);
//     pumpStatus = "ON";
//   } else {
//     digitalWrite(PUMP_IN1, LOW);
//     digitalWrite(PUMP_IN2, LOW);
//     pumpStatus = "OFF";
//   }

//   String waterStatus = (waterLevel == HIGH) ? "Water Detected" : "Dry";

//   // === Serial Output ===
//   Serial.println("\n=========Greenhouse Status =========");
//   Serial.printf("Temperature: %.2f °C\n", temperature);
//   Serial.printf("Humidity: %.2f %%\n", humidity);
//   Serial.printf("Fan Status: %s\n", fanStatus.c_str());
//   Serial.printf("Pump Status: %s\n", pumpStatus.c_str());
//   Serial.printf("Gas Sensor: %d ppm\n", gasLevel);
//   Serial.printf("Water Sensor: %s\n", waterStatus.c_str());
//   Serial.printf("Soil Moisture: %d %%\n", soilMoisture);

//   // === Firebase Upload ===
//   if (Firebase.ready()) {
//     time_t now = time(nullptr);
//     struct tm* timeinfo = localtime(&now);
//     char timestamp[30];
//     strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);

//     FirebaseJson json;

//     char tempBuffer[10];
//     char humBuffer[10];
//     char gasBuffer[20];
//     char soilBuffer[10];

//     snprintf(tempBuffer, sizeof(tempBuffer), "%.2f °C", temperature);
//     snprintf(humBuffer, sizeof(humBuffer), "%.2f %%", humidity);
//     snprintf(gasBuffer, sizeof(gasBuffer), "%d ppm", gasLevel);
//     snprintf(soilBuffer, sizeof(soilBuffer), "%d %%", soilMoisture);

//     json.set("temperature", String(tempBuffer));
//     json.set("humidity", String(humBuffer));
//     json.set("fanStatus", fanStatus);
//     json.set("pumpStatus", pumpStatus);
//     json.set("gasLevel(ppm)", String(gasBuffer));
//     json.set("waterStatus", waterStatus);
//     json.set("soilMoisture", String(soilBuffer));
//     json.set("timestamp", String(timestamp));

//     if (Firebase.RTDB.pushJSON(&fbdo, "/Greenhouse", &json)) {
//       Serial.println("Data sent to Firebase!");
//     } else {
//       Serial.printf("Firebase push failed: %s\n", fbdo.errorReason().c_str());
//     }
//   } else {
//     Serial.println("Firebase not ready");
//   }
// }