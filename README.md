**Home - Projeto ESP32: Sensor de Proximidade, Encoder e Display LCD com MQTT**


Descrição do Projeto
Este projeto envolve a criação de uma aplicação utilizando o ESP32, conectada a um sensor de proximidade HC-SR04, um encoder rotativo e um display LCD 16x2 com interface I2C. Os dados são enviados a um broker MQTT e exibidos no display.

O objetivo principal do projeto é monitorar a distância medida pelo sensor HC-SR04 e a posição do encoder, transmitindo essas informações via MQTT para monitoramento remoto.

Funcionalidades:
* Medição de distância com o HC-SR04.
* Leitura da rotação e do botão de um encoder rotativo.
* Exibição das leituras em um display LCD 16x2 com I2C.
* Envio de leituras do sensor e do encoder para um broker MQTT.
* Status de conexão Wi-Fi e MQTT exibido no display.

Tecnologias Utilizadas:
* ESP32
* Sensor de proximidade HC-SR04
* Encoder rotativo (HW-040)
* Display LCD 16x2 com I2C
* MQTT (usando o broker público hivemq.com)
* Wi-Fi

**1. Hardware - Componentes do Projeto**

1.1. ESP32
O ESP32 é o microcontrolador principal que gerencia as leituras dos sensores e a comunicação com o broker MQTT.

1.2. Sensor de Proximidade HC-SR04
O HC-SR04 é um sensor ultrassônico utilizado para medir a distância de objetos em centímetros.

![image](https://github.com/user-attachments/assets/78d2e731-bb16-4f0d-a584-86f5e2f74bad)

1.3. Encoder Rotativo (HW-040)
O encoder rotativo é usado para detectar rotação e pressionamentos de botão. Os dados são enviados via MQTT.

![image](https://github.com/user-attachments/assets/35e5d59a-d094-49c7-abed-162818af075b)

1.4. Display LCD 16x2 com I2C
O display LCD exibe as informações sobre a distância medida, a posição do encoder e o status da conexão Wi-Fi e MQTT.

![image](https://github.com/user-attachments/assets/5c61bd90-b8c9-4023-a011-d4c03eeeca79)


[https://wokwi.com/projects/411086077824230401](https://wokwi.com/projects/411095782632330241)

![image](https://github.com/user-attachments/assets/10f4ae60-7706-4ef4-9ff1-579e9a0cd724)

**2. Software - Configuração e Instalação**

2.1. Dependências
Antes de compilar e enviar o código para o ESP32, é necessário instalar as seguintes bibliotecas:

PubSubClient: para a comunicação MQTT.
LiquidCrystal_I2C: para controlar o display LCD via I2C.
WiFi.h: para conectividade Wi-Fi no ESP32.

**2.2. Passos de Instalação**

* Clone o repositório do projeto do GitHub:

git clone https://github.com/LucasFerreira2D/ESP32-MQTT-Proximity-Encoder.git

Instalar as bibliotecas necessárias:

* Na IDE Arduino, vá para: Sketch -> Incluir Biblioteca -> Gerenciar Bibliotecas...
Pesquise por "PubSubClient" e "LiquidCrystal_I2C", e instale as bibliotecas.
Configuração do Wi-Fi e MQTT:

* No arquivo de código, altere os valores de SSID e PASSWORD para os dados da sua rede Wi-Fi:

const char* SSID = "SeuSSID";
const char* PASSWORD = "SuaSenhaWiFi";

O broker MQTT já está configurado para o broker público hivemq.com, mas você pode mudar conforme necessário:

const char* BROKER_MQTT = "broker.hivemq.com";
int BROKER_PORT = 1883;

Upload do código para o ESP32:

* Conecte o ESP32 à sua máquina via USB e faça o upload do código através da IDE Arduino.


**3. Funcionalidades**
3.1. Leitura de Proximidade
O HC-SR04 mede a distância em intervalos de 1 segundo. Se a leitura for diferente da anterior, os dados são:

* Exibidos no display LCD.
* Enviados para o broker MQTT no tópico ESP32_PROXIMIDADE_IOT_IFSP.

3.2. Leitura do Encoder Rotativo
O encoder rotativo monitora a rotação (sentido horário/anti-horário) e o botão de pressão. Se o botão for pressionado, o contador é zerado. As leituras do encoder são enviadas para o broker MQTT no tópico ESP32_GIRO_IOT_IFSP.

3.3. Exibição no Display LCD
O display LCD 16x2 exibe:

* Status de conexão Wi-Fi.
* Status de conexão com o broker MQTT.
* Leituras de proximidade e do encoder.

**4. MQTT - Monitoramento via MQTT**

4.1. Broker MQTT Utilizado
* O projeto utiliza o broker público HiveMQ:

URL: broker.hivemq.com
Porta: 1883

4.2. Tópicos MQTT
Proximidade:
* Tópico: ESP32_PROXIMIDADE_IOT_IFSP
* Dados: Distância medida pelo sensor HC-SR04.

Giro (Encoder):
* Tópico: ESP32_GIRO_IOT_IFSP
* Dados: Posição atual do encoder rotativo.

4.3. Teste de Publicação/Assinatura
Você pode usar o cliente MQTT.fx ou o Mosquitto para testar a comunicação com o broker MQTT:

**Assinando o tópico de proximidade:**
mosquitto_sub -h broker.hivemq.com -t ESP32_PROXIMIDADE_IOT_IFSP

**Assinando o tópico de giro:**
mosquitto_sub -h broker.hivemq.com -t ESP32_GIRO_IOT_IFSP
