#include <WiFi.h>
#include <PubSubClient.h>
#include "Thermistor.h"

#define LED_PIN 17
#define TEMP_PIN 32

#define MAX_READINGS 10
#define TEMP_INCREASE_TRESHOLD 5

// Configuracoes de rede
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// MQTT config
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_topic = "testtopic/fire_publish";
const char* mqtt_id = "arduinoClient";


WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Variaveis relacionadas as medicoes de temperatura
int tempReadings[MAX_READINGS];
int validReadingsCount = 0;
int readIndex = 0;
bool lastState = false;
Thermistor temp(TEMP_PIN);

void setup() {
  Serial.begin(9600);

  analogReadResolution(10);
  pinMode(TEMP_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  initWiFi();
  initMQTT();
}

void initWiFi()
{
  Serial.print("Conectando-se ao Wi-Fi");
  WiFi.begin(ssid, password, 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Conectado!");
}

void initMQTT()
{
  mqttClient.setServer(mqtt_server, mqtt_port);
}

void reconnect() {
  while(!mqttClient.connected()) {
    Serial.print("* Tentando se conectar ao Broker MQTT: ");
    Serial.println(mqtt_server);
    if (mqttClient.connect(mqtt_id)) {
      Serial.println("Conectado com sucesso ao broker MQTT!");
    } else {
      Serial.println("Falha ao reconectar no broker.");
      Serial.println("Nova tentativa de conexao em 2 segundos.");
      delay(2000);
    }
  }
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }

  mqttClient.loop();

  if (validReadingsCount < MAX_READINGS) {
    validReadingsCount++;
  }

  int temperature = temp.getTemp();
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println("*C");

  appendReading(tempReadings, MAX_READINGS, temperature);
  bool currentState = checkTemperatureIncrease(validReadingsCount);
  if (currentState) {
    if (!lastState) {
      mqttClient.publish(mqtt_topic, "Alerta de risco de incêndio!");
    }
    digitalWrite(LED_PIN, HIGH); 
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  lastState = currentState;
  delay(1000);
}

void appendReading(int arr[], int size, int newValue) {
  for (int i = 0; i < size; i++) {
    arr[i] = arr[i+1];
  }

  arr[size  - 1] = newValue;
}

bool checkTemperatureIncrease(int count) {
  if (count < 2) {
    Serial.println("Não foram feitas medições suficientes");
  }

  int init = MAX_READINGS - validReadingsCount;
  int minTemp = tempReadings[init];
  int maxIncrease = 0;

  for (int i = init + 1; i < MAX_READINGS; i++) {
    if (tempReadings[i] < minTemp) {
      minTemp = tempReadings[i];
    }

    if (tempReadings[i] - minTemp > maxIncrease) {
      maxIncrease = tempReadings[i] - minTemp;
    }
  }

  Serial.print("Aumento: ");
  Serial.print(maxIncrease);
  Serial.println("*C");

  return maxIncrease >= TEMP_INCREASE_TRESHOLD;
}