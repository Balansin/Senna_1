#include <WiFi.h>
#include <PubSubClient.h>
#include <PL_ADXL355.h>
#include <SPI.h>

// ====== CONFIGURAÇÃO DE REDE E MQTT ======
const char* ssid = "XXX";
const char* password = "XXX";
const char* mqtt_server = "XXX";
const char* mqtt_topic = "esp32/adxl355";

// ====== REGISTRO DO ESP32 ======
WiFiClient espClient;
PubSubClient client(espClient);

// #ToDo - por que long (?)
long lastMsg = 0;

// ====== LEDS ======
#define LED_WIFI_WAIT 2  // Pisca enquanto conecta
#define LED_WIFI_OK   3  // Pisca 3 vezes após conexão
#define LED_WIFI_TESTE 1  // Pisca enquanto conecta

// ====== SENSOR ADXL355 ======
#define CS_PIN 5
PL::ADXL355 adxl355;
auto range = PL::ADXL355_Range::range2g;

// Both ADXL355 and ADXL355_Range defined within namespace PL


// ====== Função de conexão WiFi com LED de status ======
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  pinMode(LED_WIFI_WAIT, OUTPUT);
  pinMode(LED_WIFI_OK, OUTPUT);


  // ToDo - qual condicao esta sendo verificada?
  // i.e qual funcao esta sendo invocada e quais dispositivos esao sendo checados;
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_WIFI_WAIT, HIGH);
    delay(200);
    digitalWrite(LED_WIFI_WAIT, LOW);
    delay(300);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  // Pisca LED de confirmação
  for (int i = 0; i < 2; i++) {
    digitalWrite(LED_WIFI_OK, HIGH);
    delay(100);
    digitalWrite(LED_WIFI_OK, LOW);
    delay(1000);
  }
}

// ====== Reconecta ao MQTT se desconectar ======
void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao broker MQTT...");
    if (client.connect("ESP32Client_ADXL355")) {
      Serial.println("Conectado!");
      delay(2000);
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" — tentando em 5s.");

      // Pisca o LED 3 vezes rapidamente
      for (int i = 0; i < 3; i++) {
        digitalWrite(LED_WIFI_WAIT, HIGH);
        delay(100);               // LED aceso por 100 ms
        digitalWrite(LED_WIFI_WAIT, LOW);
        delay(200);               // LED apagado por 100 ms
      }

      // Espera 5 segundos antes de tentar reconectar
      delay(5000);
    }
  }
}


// ====== SETUP ADXL 355 ======
void setup() {
  Serial.begin(115200);
  while (!Serial);


  // ====== Inicio da coleta de dados ======

  pinMode(LED_WIFI_WAIT, OUTPUT);
  pinMode(LED_WIFI_OK, OUTPUT);
  digitalWrite(LED_WIFI_WAIT, LOW);
  digitalWrite(LED_WIFI_OK, LOW);

  // Inicia o sensor
  adxl355.beginSPI(CS_PIN);
  adxl355.setRange(range);
  adxl355.enableMeasurement();

  // Conexão WiFi e MQTT
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  Serial.println("X\tY\tZ");
}

// ====== LOOP PARA COLETA DE DADOS ======
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 1) {  // A cada 5 segundos
    lastMsg = now;

    // Lê acelerações
    auto acc = adxl355.getAccelerations();

    // Mostra no serial
    Serial.print(acc.x); Serial.print("\t");
    Serial.print(acc.y); Serial.print("\t");
    Serial.println(acc.z);

    // Publica no tópico MQTT
    char payload[100];
    snprintf(payload, sizeof(payload),
             "{\"x\": %.4f, \"y\": %.4f, \"z\": %.4f}", acc.x, acc.y, acc.z);
    client.publish(mqtt_topic, payload);
  }
}
