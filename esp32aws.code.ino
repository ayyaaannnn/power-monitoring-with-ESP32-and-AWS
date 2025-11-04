/*************************************************
 * ESP32 Voltage + Current Measurement + AWS IoT
 * Works with 9–14 V supply, LED or DC motor load
 *************************************************/
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"

// --- Measurement setup ---
const int pinVin = 34;      // Voltage divider input
const int pinACS = 35;      // ACS712 output (through 3.3k/3.3k divider)

// Divider values for Vin
const float R1 = 10000.0;   // Top resistor (10k)
const float R2 = 3000.0;    // Bottom resistor (3k)

// Calibration factor for ESP32 ADC
const float ADC_CAL = 1.25;   // ~25% correction for ESP32 ADC nonlinearity

// ACS712 details (5A version)
const float ACS_SENSITIVITY = 0.185;   // V/A (typical for ACS712-05B)
float acsZeroSensor = 0.0;             // Actual ACS712 OUT voltage at 0A

// --- AWS IoT setup ---
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

WiFiClientSecure net;
PubSubClient client(net);

// Helper: average multiple ADC samples
float readAverage(int pin, int samples = 100) {
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(pin);
  }
  return (float)sum / samples;
}

// Handle incoming AWS messages
void messageHandler(char* topic, byte* payload, unsigned int length) {
  Serial.print("Incoming message [");
  Serial.print(topic);
  Serial.print("]: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// Publish JSON with Vin + Current
void publishMessage(float vin, float currentA) {
  StaticJsonDocument<200> doc;
  doc["voltage"] = vin;
  doc["current"] = currentA;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void connectAWS() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected");

  // Load AWS certificates from secrets.h
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Correct port for MQTT over TLS
  client.setServer(AWS_IOT_ENDPOINT, 8883);
  client.setCallback(messageHandler);

  Serial.println("Connecting to AWS IoT...");
  while (!client.connect(THINGNAME)) {
    Serial.print("Connection failed, state = ");
    Serial.println(client.state());  // Debug reason (-4 bad creds, -5 not authorized)
    delay(2000);
  }

  Serial.println("AWS IoT Connected!");
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  // --- Calibrate ACS712 zero offset (no load connected!) ---
  float rawZero = readAverage(pinACS);
  float vAtJunctionZero = (rawZero / 4095.0) * 3.3;
  acsZeroSensor = vAtJunctionZero * 2.0;
  Serial.print("ACS Zero Offset = ");
  Serial.print(acsZeroSensor, 3);
  Serial.println(" V");

  connectAWS();
}

void loop() {
  // --- Voltage measurement ---
  float rawVin = readAverage(pinVin);
  float vADC = (rawVin / 4095.0) * 3.3;
  float vin = vADC * ((R1 + R2) / R2) * ADC_CAL;

  // --- Current measurement ---
  float rawACS = readAverage(pinACS);
  float vAtJunction = (rawACS / 4095.0) * 3.3;
  float vSensor = vAtJunction * 2.0;
  float currentA = (acsZeroSensor - vSensor) / ACS_SENSITIVITY;

  // Print readings
  Serial.print("Vin = ");
  Serial.print(vin, 3);
  Serial.print(" V    |    I = ");
  Serial.print(currentA, 3);
  Serial.print(" A    |    vACS = ");
  Serial.print(vSensor, 3);
  Serial.println(" V");

  // Publish to AWS IoT
  publishMessage(vin, currentA);

  client.loop();
  delay(1000);
}
