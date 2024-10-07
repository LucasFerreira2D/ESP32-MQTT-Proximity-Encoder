#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Configuração do display I2C
#define I2C_ADDR    0x27
#define LCD_COLUMNS 16
#define LCD_LINES   2

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

// Definir pinos do sensor HC-SR04
#define TRIG_PIN 5   // Pino TRIG do sensor HC-SR04
#define ECHO_PIN 18  // Pino ECHO do sensor HC-SR04

// Definir pinos do sensor de giro (Encoder)
#define CLK 19  // Pino do sinal CLK do encoder
#define DT 21   // Pino do sinal DT do encoder
#define SW 4    // Pino do botão do encoder (pressionado para zerar)

// Definir tópicos MQTT e ID do cliente
#define TOPICO_SUBSCRIBE_PROXIMIDADE "ESP32_PROXIMIDADE_IOT_IFSP"
#define TOPICO_PUBLISH_PROXIMIDADE   "ESP32_PROXIMIDADE_IOT_IFSP"
#define TOPICO_SUBSCRIBE_GIRO        "ESP32_GIRO_IOT_IFSP"
#define TOPICO_PUBLISH_GIRO          "ESP32_GIRO_IOT_IFSP"
#define ID_MQTT  "INCB_Cliente_MQTT"

// Configurações de Wi-Fi e MQTT
const char* SSID = "Wokwi-GUEST"; 
const char* PASSWORD = ""; 
const char* BROKER_MQTT = "broker.hivemq.com";
int BROKER_PORT = 1883;

WiFiClient espClient;
PubSubClient MQTT(espClient);

// Variáveis para o sensor de giro (encoder)
int counter = 0;         // Contador de rotações do encoder
int lastStateCLK;        // Estado anterior do CLK
int currentStateCLK;     // Estado atual do CLK
unsigned long lastProximityPublish = 0;  // Controla quando enviar a leitura de proximidade
unsigned long proximityInterval = 1000; // Intervalo de 1 segundo para verificar proximidade
long previousProximity = -1;  // Valor anterior da proximidade para comparação

// Funções de Inicialização e Reconexão
void init_serial(void);
void init_wifi(void);
void init_mqtt(void);
void reconnect_wifi(void); 
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void verifica_conexoes_wifi_mqtt(void);

// Função para ler o sensor de proximidade HC-SR04
long readProximity() {
  // Enviar pulso TRIG
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Ler o tempo de retorno do pulso ECHO
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Calcular a distância em centímetros
  long distance = duration * 0.034 / 2;

  return distance;
}

// Função para ler o sensor de giro (encoder)
void readEncoder() {
  currentStateCLK = digitalRead(CLK);

  // Se o estado do CLK mudou
  if (currentStateCLK != lastStateCLK) {
    int currentStateDT = digitalRead(DT);

    // Girando no sentido horário
    if (currentStateDT != currentStateCLK) {
      counter++;
    } else {  // Girando no sentido anti-horário
      counter--;
    }
    Serial.print("Posição do encoder: ");
    Serial.println(counter);

    // Enviar a leitura do encoder via MQTT
    String message = String(counter);
    MQTT.publish(TOPICO_PUBLISH_GIRO, message.c_str());

    // Exibir a posição do encoder na linha 2 do display
    lcd.setCursor(0, 1);
    lcd.print("Encoder: ");
    lcd.print(counter);
    lcd.print("   "); // Limpar caracteres adicionais
  }

  // Verificar se o botão do encoder foi pressionado (zerar contador)
  if (digitalRead(SW) == LOW) {
    counter = 0;  // Zerar o contador do encoder
    Serial.println("Botão do encoder pressionado! Contador zerado.");
    
    // Enviar o valor zero via MQTT
    String message = String(counter);
    MQTT.publish(TOPICO_PUBLISH_GIRO, message.c_str());

    // Exibir no display
    lcd.setCursor(0, 1);
    lcd.print("Encoder: 0   ");

    delay(500);  // Debounce para evitar múltiplas leituras rápidas
  }

  lastStateCLK = currentStateCLK;
}

// Configurações Iniciais
void setup() 
{
    init_serial();
    init_wifi();
    init_mqtt();

    // Configurar pinos do HC-SR04
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // Configurar pinos do sensor de giro (encoder)
    pinMode(CLK, INPUT);
    pinMode(DT, INPUT);
    pinMode(SW, INPUT_PULLUP);  // Configurar o botão com pull-up interno
    lastStateCLK = digitalRead(CLK);

    // Inicializar o display LCD I2C
    Wire.begin(23, 22);  // Configurar manualmente os pinos SDA (23) e SCL (22)
    lcd.init();          // Inicializar o display LCD
    lcd.backlight();     // Ativar a luz de fundo
    lcd.setCursor(0, 0);
    lcd.print("Iniciando...");
}

void init_serial() 
{
    Serial.begin(115200);
}

void init_wifi(void) 
{
    delay(10);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Conectando WiFi");
    lcd.setCursor(0, 1);
    lcd.print(SSID);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    reconnect_wifi();
}

void init_mqtt(void) 
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);
    MQTT.setCallback(mqtt_callback);
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
    String msg;
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }
    Serial.print("[MQTT] Mensagem recebida: ");
    Serial.println(msg);
}

void reconnect_mqtt(void) 
{
    while (!MQTT.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Conectando MQTT");
        if (MQTT.connect(ID_MQTT)) 
        {
            Serial.println("Conectado ao broker MQTT!");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("MQTT Conectado");
            MQTT.subscribe(TOPICO_SUBSCRIBE_PROXIMIDADE);
            MQTT.subscribe(TOPICO_SUBSCRIBE_GIRO);
        } 
        else
        {
            Serial.println("Falha ao conectar ao broker.");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Falha MQTT");
            delay(2000);
        }
    }
}

void reconnect_wifi() 
{
    if (WiFi.status() == WL_CONNECTED)
        return;

    WiFi.begin(SSID, PASSWORD);

    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Conectado ao Wi-Fi.");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Conectado");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
}

void verifica_conexoes_wifi_mqtt(void)
{
    reconnect_wifi();
    if (!MQTT.connected()) 
        reconnect_mqtt();
}

void loop() 
{   
    verifica_conexoes_wifi_mqtt();

    // Ler e enviar os dados de proximidade se diferente do anterior
    unsigned long currentMillis = millis();
    if (currentMillis - lastProximityPublish >= proximityInterval) {
        lastProximityPublish = currentMillis;

        long distance = readProximity();

        if (distance != previousProximity) {
            Serial.print("Distância: ");
            Serial.print(distance);
            Serial.println(" cm");

            // Exibir a distância na linha 1 do display
            lcd.setCursor(0, 0);
            lcd.print("Dist: ");
            lcd.print(distance);
            lcd.print(" cm   "); // Limpar caracteres adicionais

            // Enviar a distância via MQTT
            String mensagem = String(distance);
            MQTT.publish(TOPICO_PUBLISH_PROXIMIDADE, mensagem.c_str());

            previousProximity = distance;
        }
    }

    // Leitura do sensor de giro (encoder)
    readEncoder();

    MQTT.loop();
}

