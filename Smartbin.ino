#include <WiFi.h>              // Biblioteca para conectar o ESP32 ao WiFi
#include <PubSubClient.h>      // Biblioteca para comunicação MQTT

// ================== CONFIGURAÇÕES ==================

// Credenciais padrão de WiFi
const char* default_SSID = "Wokwi-GUEST";
const char* default_PASSWORD = "";

// Endereço do broker MQTT
const char* default_BROKER_MQTT = "172.183.151.189";
const int default_BROKER_PORT = 1883;

// Tópicos FIWARE
const char* default_TOPICO_SUBSCRIBE = "/TEF/bin001/cmd";     // comandos do FIWARE
const char* default_TOPICO_PUBLISH_1 = "/TEF/bin001/attrs";   // publicação de atributos
const char* default_TOPICO_PUBLISH_2 = "/TEF/bin001/attrs/d"; // publicação da distância

// ID do dispositivo no MQTT/FIWARE
const char* default_ID_MQTT = "fiware_001";

// Prefixo FIWARE para comandos
const char* topicPrefix = "bin001";

// Variáveis que podem ser alteradas em runtime
char* SSID = const_cast<char*>(default_SSID);
char* PASSWORD = const_cast<char*>(default_PASSWORD);
char* BROKER_MQTT = const_cast<char*>(default_BROKER_MQTT);
int BROKER_PORT = default_BROKER_PORT;

char* TOPICO_SUBSCRIBE = const_cast<char*>(default_TOPICO_SUBSCRIBE);
char* TOPICO_PUBLISH_1 = const_cast<char*>(default_TOPICO_PUBLISH_1);
char* TOPICO_PUBLISH_2 = const_cast<char*>(default_TOPICO_PUBLISH_2);
char* ID_MQTT = const_cast<char*>(default_ID_MQTT);

// ================== ULTRASSÔNICO ==================
// Pinos do sensor HC-SR04
#define TRIG 5
#define ECHO 18

// ================== MQTT / WIFI ==================
WiFiClient espClient;    // Objeto de conexão WiFi
PubSubClient MQTT(espClient); // Cliente MQTT usando WiFi

char EstadoSaida = '0'; // estado para FIWARE (feedback de comandos)

// ================== Funções ==================

// Inicializa o monitor serial
void initSerial() {
  Serial.begin(115200);
}

// Conecta no WiFi
void initWiFi() {
  Serial.println("------ Conexao WIFI ------");
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// Configura o MQTT
void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT); // define broker e porta
  MQTT.setCallback(mqtt_callback);          // função callback ao receber mensagens
}

// Reconecta ao MQTT caso desconecte
void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.print("Conectando ao broker MQTT...");

    if (MQTT.connect(ID_MQTT)) {
      Serial.println("Conectado!");
      MQTT.subscribe(TOPICO_SUBSCRIBE); // ouve os comandos do FIWARE
    } else {
      Serial.println("Falhou. Tentando novamente...");
      delay(2000);
    }
  }
}

// Verifica WiFi e MQTT, reconectando caso necessário
void VerificaConexoes() {
  if (!MQTT.connected()) reconnectMQTT();
  if (WiFi.status() != WL_CONNECTED) initWiFi();
}

// ================== CALLBACK MQTT ==================
// Função executada quando chega uma mensagem MQTT
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String msg;

  // Converte a mensagem recebida em texto
  for (int i = 0; i < length; i++) msg += (char)payload[i];

  Serial.print("CMD Recebido: ");
  Serial.println(msg);

  // Padrão FIWARE: bin001@on|   bin001@off|
  String onTopic  = String(topicPrefix) + "@on|";
  String offTopic = String(topicPrefix) + "@off|";

  // Liga/desliga conforme comando
  if (msg == onTopic)  EstadoSaida = '1';
  if (msg == offTopic) EstadoSaida = '0';

  // Publica sempre o estado após o comando ser recebido
  String s = "s|" + String(EstadoSaida);
  MQTT.publish(TOPICO_PUBLISH_1, s.c_str());
}

// ================== ULTRASSÔNICO ==================
// Lê distância do sensor HC-SR04
float readDistance() {
  // Limpa trigger
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  // Pulso de 10us para ativar o sensor
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  // Mede o tempo do eco
  long duration = pulseIn(ECHO, HIGH);

  // Converte tempo para distância em cm
  float distance = (duration * 0.034) / 2;

  // Validação do sensor
  if (distance <= 0 || distance > 400) return -1;
  return distance;
}

// ================== SETUP ==================
void setup() {
  initSerial();
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  initWiFi();   // conecta no WiFi
  initMQTT();   // configura MQTT

  delay(2000);

  // Mensagem inicial FIWARE informando que o dispositivo está ativo
  MQTT.publish(TOPICO_PUBLISH_1, "s|on");
}

// ================== LOOP ==================
void loop() {
  VerificaConexoes(); // mantém WiFi e MQTT vivos
  MQTT.loop();        // processa mensagens recebidas

  static unsigned long last = 0;
  
  // Publica distância a cada 5 segundos
  if (millis() - last >= 5000) {
    last = millis();

    float d = readDistance();

    if (d < 0) {
      Serial.println("[ERRO] Leitura inválida");
      return;
    }

    char bufD[10];
    dtostrf(d, 0, 2, bufD); // converte float → char*

    Serial.print("[PUB] distance = ");
    Serial.println(bufD);

    // Publica no tópico FIWARE correto
    MQTT.publish(TOPICO_PUBLISH_2, bufD);
  }
}
