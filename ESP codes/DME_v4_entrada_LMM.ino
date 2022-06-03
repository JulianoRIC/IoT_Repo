//Programa: Modulo ESP32 Wifi com MQTT
//
//Autor : Gabriel Manoel da Silva - DAS/UFSC

const int REPORT_INTERVAL = 30*1000; //a cada 10 s 

#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
//Adicionando bibliotecas
//#include <WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <Time.h>
#include<ADS1115_WE.h> 
#include<Wire.h>

//Endereco do modulo ADS1115
#define I2C_ADDRESS 0x48

#define D5 14
#define LED_1 16

ADS1115_WE adc(I2C_ADDRESS);

//Add nome da rede Wifi
 char* ssid = "LMM";
// Add senha do Wifi
 char* password =  "mecatronica";

// MQTT
#define TOPICO_PUBLISH   "dev/lmm/esp32/tf/000"       // tópico MQTT para envio de informações do ESP32 para broker MQTT
#define ID_MQTT  "ESP32 TF 000"                       // id do cliente MQTT (deve ser único e seguir o padrão especificado
const char* BROKER_MQTT = "150.162.14.51";            // URL do broker MQTT utilizado
int BROKER_PORT = 1883;                               // porta do broker MQTT


unsigned long lastSend;

WiFiClient espClient;                                 // criação do objeto espClient para
PubSubClient MQTT(espClient);

void init_mqtt(void);
void setup() 
{
  Wire.begin();
  Serial.begin(9600);
  pinMode(LED_1, OUTPUT);     // Initialize the LED_BUILTIN pin as an output

  // Configura Wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password); // Conexao com o Wifi
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Iniciando conexao com a rede WiFi..");
  }
  Serial.println("Conectado na rede WiFi!");
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  init_mqtt();
   
  if(!adc.init())
  {
    Serial.println("ADS1115 nao conectado!");
  }
  
  adc.setVoltageRange_mV(ADS1115_RANGE_1024);
  adc.setConvRate(ADS1115_475_SPS); //uncomment if you want to change the default

  adc.setMeasureMode(ADS1115_CONTINUOUS);

  Serial.println("ADS1115 com ESP8266 NodeMCU - Leitura continua");
  Serial.println("Todos os valores em volts!");
  Serial.println();
}

void loop() {
  
 
  if ( millis() - lastSend > REPORT_INTERVAL ) 
  { getAndSendData();
    lastSend = millis();
    digitalWrite(LED_1, !digitalRead(LED_1));
    ESP.wdtFeed();
  }
  MQTT.loop();
  
}


double calcIrms(unsigned int Number_of_Samples,ADS1115_MUX channel, int canal )
{
  double sqI=0;
  double sumI=0; 
  adc.setCompareChannels(channel); //periodo de aproximadamente 660 us no loop
  for (unsigned int n = 0; n < Number_of_Samples; n++)
  {
    int sampleI =adc.getResult_mV();

    // Root-mean-square method current
    // 1) square current values
    sqI = sampleI * sampleI;
    // 2) sum
    sumI += sqI;
    digitalWrite(D5, !digitalRead(D5));
  }

  
  double Irms = 1 * sqrt(sumI / Number_of_Samples);
  if(canal==1)
    Irms*=(2/15.4);
  if(canal==2)
    Irms*=(2/15.5);
  if(canal==3)
    Irms*=(2 /15.2);

  return Irms;
}

void reconnect_mqtt(void)
{
  while (!MQTT.connected())
  {
    Serial.print("* Tentando se conectar ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    if (MQTT.connect(ID_MQTT))
    {
      Serial.println("Conectado com sucesso ao broker MQTT!");
      //MQTT.subscribe(TOPICO_SUBSCRIBE);
    }
    else
    {
      Serial.println("Falha ao reconectar no broker.");
      Serial.println("Havera nova tentatica de conexao em 2s");
      delay(2000);
    }

    
  }
  IPAddress ip (150, 162, 14, 51); // The remote ip to ping
    bool ret = Ping.ping(ip);
    if(ret)
      Serial.println("Sucesso Ping Google");
    else
      Serial.println("Falha Ping Google!!!");

}
// inicia mqtt
void init_mqtt(void)
{
  /* informa a qual broker e porta deve ser conectado */
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  /* atribui função de callback (função chamada quando qualquer informação do
    tópico subescrito chega) */
  //MQTT.setCallback(mqtt_callback);
}

// Função que pega as medicoes e envia para o broker
void getAndSendData()
{


  float IRMS1 = 0.0;
  float IRMS2 = 0.0;
  float IRMS3 = 0.0;
  //voltage = readChannel(ADS1115_COMP_0_3);
  IRMS1 = calcIrms(1000, ADS1115_COMP_0_3,1); //500 ~~340 ms
  Serial.print("IRMS 1:");
  Serial.println(IRMS1);

  IRMS2 = calcIrms(1000, ADS1115_COMP_1_3,2); //500 ~~340 ms
  Serial.print("IRMS 2:");
  Serial.println(IRMS2);

  IRMS3 = calcIrms(1000, ADS1115_COMP_2_3,3); //500 ~~340 ms
  Serial.print("IRMS 3:");
  Serial.println(IRMS3);
  
  String payload = "{";
 
  payload += "\"corrente_1\":"; payload += IRMS1; payload += ",";
  payload += "\"corrente_2\":"; payload += IRMS2; payload += ",";
  payload += "\"corrente_3\":"; payload += IRMS3; payload += "}";

  reconnect_mqtt();

  // Send payload to MQTT

  char mensagem[300];
  payload.toCharArray(mensagem, 300 );
  int succes = MQTT.publish(TOPICO_PUBLISH, mensagem);
  Serial.print("retorno:");
  Serial.println(succes);
  Serial.println(mensagem);

  delay(100);
}
