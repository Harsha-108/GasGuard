#include <WiFi.h>
#include <HTTPClient.h>

// WiFi Credentials
const char* ssid = "project";
const char* password = "project007";

// ThingSpeak API
const char* server = "http://api.thingspeak.com/update";
String apiKey = "H8WP6AJFT410LCQ9"; // Your ThingSpeak API Key

// GSM Module Pins
#define RXD2 16 // Connect to TX of GSM Module
#define TXD2 17 // Connect to RX of GSM Module

// MQ Sensor Pins (Raw ADC readings)
const int mq2Pin = 34; // Connected to MQ2 sensor
const int mq4Pin = 35; // Connected to MQ4 sensor
const int mq9Pin = 32; // Connected to MQ9 sensor

// LED Pins
const int mq2LedPin = 25; // LED for MQ2 related alert
const int mq4LedPin = 26; // LED for MQ4 related alert
const int mq9LedPin = 27; // LED for MQ9 related alert
const int svmAlertLedPin = 2; // Dedicated LED for SVM "Non-False Alarm"

// Buzzer Pin
const int buzzerPin = 14;

// --- SVM Model Parameters (UPDATE THESE FROM PYTHON SCRIPT OUTPUT) ---
// These are EXAMPLE values. Replace them with the actual values from your trained model.
// Number of features used by the SVM model
const int NUM_SVM_FEATURES = 8;

// Scaler parameters (mean and scale for each of the 8 features)
// Order: LPG, Propane, Hydrogen, Smoke, Methane(MQ2), Methane(MQ4), CNG, CO
const float scaler_means[NUM_SVM_FEATURES] = {
  [142.40333333, 113.9236,     170.88426667, 213.60586667,  71.20226667,
    222.08346667, 155.45786667, 429.90466667
};
const float scaler_scales[NUM_SVM_FEATURES] = {
  189.06130002, 151.24915203, 226.873879,   283.59110089,  94.53084038,
  117.64060023,  82.34863537, 289.2680366
};

// SVM model weights (coefficients for each of the 8 scaled features)
const float svm_weights[NUM_SVM_FEATURES] = {
  0.00880251, 0.00874205, 0.008842,   0.00860357, 0.00876678, 0.10061979,
  0.10068662, 1.97990444
};

// SVM model intercept
const float svm_intercept = -0.4449249879323717; // EXAMPLE VALUE
// --- End SVM Model Parameters ---


// Thresholds for individual sensor LEDs (optional, can still be used for indication)
const int mq2RawThreshold = 2000; // Example raw ADC threshold
const int mq4RawThreshold = 2000; // Example raw ADC threshold
const int mq9RawThreshold = 2000; // Example raw ADC threshold

// Constants for sensor calculations (resistance, etc.)
float mq2RL = 10.0; // Load resistance in kOhms for MQ2
float mq4RL = 20.0; // Load resistance in kOhms for MQ4
float mq9RL = 10.0; // Load resistance in kOhms for MQ9

// Array to hold the 8 feature values for SVM input
float sensor_features_for_svm[NUM_SVM_FEATURES];


void setup() {
  Serial.begin(115200);       // Debugging serial
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); // UART2 for GSM communication

  // Initialize WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  int wifi_retries = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_retries < 20) {
    delay(500);
    Serial.print(".");
    wifi_retries++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi. Continuing without it.");
  }

  // Pin Modes
  pinMode(mq2LedPin, OUTPUT);
  pinMode(mq4LedPin, OUTPUT);
  pinMode(mq9LedPin, OUTPUT);
  pinMode(svmAlertLedPin, OUTPUT); // SVM specific alert LED
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(mq2LedPin, LOW);
  digitalWrite(mq4LedPin, LOW);
  digitalWrite(mq9LedPin, LOW);
  digitalWrite(svmAlertLedPin, LOW);
  digitalWrite(buzzerPin, LOW);

  Serial.println("GasGuard with SVM Initialized.");
  Serial.println("IMPORTANT: Ensure SVM parameters (means, scales, weights, intercept) are correctly updated from the Python script!");
}

// Function to calculate concentrations from MQ2 raw value
// Stores results in sensor_features_for_svm array
void calculateMQ2Features(int rawValue) {
  if (rawValue == 0) rawValue = 1; // Avoid division by zero
  float rs_ro_ratio = (4095.0 / rawValue - 1.0); // Assuming Ro is such that this gives a base for ppm
                                                // This is a simplification. True ppm requires calibration curves.
                                                // The Python model was trained on 'fieldX' values, which are assumed to be these direct calculations.
  // Based on your feed.json and original ESP code:
  // field1: LPG, field2: Propane, field3: Hydrogen, field4: Smoke, field5: Methane(MQ2)
  sensor_features_for_svm[0] = rs_ro_ratio * 100; // LPG (example factor)
  sensor_features_for_svm[1] = rs_ro_ratio * 80;  // Propane
  sensor_features_for_svm[2] = rs_ro_ratio * 120; // Hydrogen
  sensor_features_for_svm[3] = rs_ro_ratio * 150; // Smoke
  sensor_features_for_svm[4] = rs_ro_ratio * 50;  // Methane (from MQ2)
}

// Function to calculate concentrations from MQ4 raw value
void calculateMQ4Features(int rawValue) {
  if (rawValue == 0) rawValue = 1;
  float rs_ro_ratio = (4095.0 / rawValue - 1.0);
  // field6: Methane(MQ4), field7: CNG
  sensor_features_for_svm[5] = rs_ro_ratio * 100; // Methane (from MQ4)
  sensor_features_for_svm[6] = rs_ro_ratio * 70;  // CNG
}

// Function to calculate concentration from MQ9 raw value
void calculateMQ9Features(int rawValue) {
  if (rawValue == 0) rawValue = 1;
  float rs_ro_ratio = (4095.0 / rawValue - 1.0);
  // field8: CO
  sensor_features_for_svm[7] = rs_ro_ratio * 60;  // CO
  // Original code had Methane from MQ9, but feed.json only has 8 fields, CO is the 8th.
}


// Function to perform SVM prediction
// Returns: 1 for "Non-False Alarm", 0 for "False Alarm"
int predict_svm(float features[]) {
  float scaled_features[NUM_SVM_FEATURES];
  float decision_value = 0;

  // 1. Scale the features
  for (int i = 0; i < NUM_SVM_FEATURES; i++) {
    if (scaler_scales[i] == 0) { // Avoid division by zero if scale is 0
        scaled_features[i] = (features[i] - scaler_means[i]);
    } else {
        scaled_features[i] = (features[i] - scaler_means[i]) / scaler_scales[i];
    }
  }

  // 2. Calculate decision value: sum(scaled_fi * coef_i) + intercept
  for (int i = 0; i < NUM_SVM_FEATURES; i++) {
    decision_value += scaled_features[i] * svm_weights[i];
  }
  decision_value += svm_intercept;

  Serial.print("SVM Decision Value: ");
  Serial.println(decision_value);

  // 3. Classify based on decision value
  if (decision_value >= 0) {
    return 1; // Non-False Alarm
  } else {
    return 0; // False Alarm
  }
}

// Function to check individual sensor raw values and light up their respective LEDs
void checkIndividualSensorLEDs(int mq2Raw, int mq4Raw, int mq9Raw) {
    digitalWrite(mq2LedPin, mq2Raw > mq2RawThreshold ? HIGH : LOW);
    digitalWrite(mq4LedPin, mq4Raw > mq4RawThreshold ? HIGH : LOW);
    digitalWrite(mq9LedPin, mq9Raw > mq9RawThreshold ? HIGH : LOW);
}


void sendSMS(String message) {
  if (WiFi.status() != WL_CONNECTED) { // Basic check, ideally GSM module has its own status
      Serial.println("SMS: WiFi not connected, assuming GSM might also have issues or is primary.");
      // If GSM is independent, this check might not be relevant for SMS.
  }
  Serial.println("Attempting to send SMS...");
  Serial2.println("AT+CMGF=1"); // Set SMS to Text Mode
  delay(200); // Increased delay
  Serial2.println("AT+CMGS=\"+919310852499\""); // Replace with recipient's number
  delay(200); // Increased delay
  Serial2.print(message);
  delay(200); // Increased delay
  Serial2.write(26); // Send SMS (Ctrl+Z)
  delay(1000); // Wait for SMS to send
  Serial.println("SMS send command issued.");
}

void loop() {
  // Read raw analog values from sensors
  int mq2RawValue = analogRead(mq2Pin);
  int mq4RawValue = analogRead(mq4Pin);
  int mq9RawValue = analogRead(mq9Pin);

  // Calculate "features" (concentrations) from raw values
  // These functions will populate the sensor_features_for_svm array
  calculateMQ2Features(mq2RawValue);
  calculateMQ4Features(mq4RawValue);
  calculateMQ9Features(mq9RawValue);

  Serial.println("\n--- Sensor Readings (Features for SVM) ---");
  Serial.print("LPG (F1): "); Serial.println(sensor_features_for_svm[0]);
  Serial.print("Propane (F2): "); Serial.println(sensor_features_for_svm[1]);
  Serial.print("Hydrogen (F3): "); Serial.println(sensor_features_for_svm[2]);
  Serial.print("Smoke (F4): "); Serial.println(sensor_features_for_svm[3]);
  Serial.print("Methane MQ2 (F5): "); Serial.println(sensor_features_for_svm[4]);
  Serial.print("Methane MQ4 (F6): "); Serial.println(sensor_features_for_svm[5]);
  Serial.print("CNG (F7): "); Serial.println(sensor_features_for_svm[6]);
  Serial.print("CO (F8): "); Serial.println(sensor_features_for_svm[7]);

  // Perform SVM prediction
  int svm_prediction = predict_svm(sensor_features_for_svm);
  String alertMessage = "Gas Alert:\n";
  bool triggerGlobalAlert = false;

  if (svm_prediction == 1) {
    Serial.println("SVM Prediction: Non-False Alarm Detected!");
    digitalWrite(svmAlertLedPin, HIGH); // Turn on SVM specific alert LED
    digitalWrite(buzzerPin, HIGH);
    alertMessage += "SVM: Non-False Alarm. ";
    triggerGlobalAlert = true;
  } else {
    Serial.println("SVM Prediction: False Alarm / Normal.");
    digitalWrite(svmAlertLedPin, LOW);
    digitalWrite(buzzerPin, LOW); // Turn off buzzer if SVM says false alarm
  }

  // Optional: Light up individual sensor LEDs based on their raw thresholds
  // This can give an indication of which sensor(s) are most active,
  // even if the SVM classifies it as a false alarm overall.
  checkIndividualSensorLEDs(mq2RawValue, mq4RawValue, mq9RawValue);
  if(digitalRead(mq2LedPin)) alertMessage += "MQ2 high. ";
  if(digitalRead(mq4LedPin)) alertMessage += "MQ4 high. ";
  if(digitalRead(mq9LedPin)) alertMessage += "MQ9 high. ";


  if (triggerGlobalAlert) {
    sendSMS(alertMessage);
    delay(2000); // Buzzer and alert duration
    digitalWrite(buzzerPin, LOW); // Ensure buzzer turns off after alert duration
                                  // svmAlertLedPin can remain ON until next "false alarm" state
  }


  // Upload Data to ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    // Construct URL with all 8 features + SVM prediction
    String url = String(server) + "?api_key=" + apiKey +
                 "&field1=" + String(sensor_features_for_svm[0]) + // LPG
                 "&field2=" + String(sensor_features_for_svm[1]) + // Propane
                 "&field3=" + String(sensor_features_for_svm[2]) + // Hydrogen
                 "&field4=" + String(sensor_features_for_svm[3]) + // Smoke
                 "&field5=" + String(sensor_features_for_svm[4]) + // Methane (MQ2)
                 "&field6=" + String(sensor_features_for_svm[5]) + // Methane (MQ4)
                 "&field7=" + String(sensor_features_for_svm[6]) + // CNG
                 "&field8=" + String(sensor_features_for_svm[7]);  // CO
    // Optionally, add SVM prediction to ThingSpeak if you have a 9th field configured
    // url += "&field9=" + String(svm_prediction);

    http.begin(url); //Specify URL
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.print("ThingSpeak Response Code: "); Serial.println(httpCode);
      // Serial.print("Payload: "); Serial.println(payload); // Uncomment for debugging
      if (httpCode == 200) {
         Serial.println("Data sent to ThingSpeak successfully.");
      } else {
         Serial.println("Error sending data to ThingSpeak.");
      }
    } else {
      Serial.print("ThingSpeak GET request failed, error: ");
      Serial.println(http.errorToString(httpCode).c_str());
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected. Cannot send data to ThingSpeak.");
  }

  delay(15000); // Delay before next round of readings (ThingSpeak free plan allows updates every 15s)
}
