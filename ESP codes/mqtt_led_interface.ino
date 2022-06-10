#include <WiFi.h>
#define DEVICE "ESP32 AC 001"
#include <PubSubClient.h>
#include <String.h>

#include <DHT.h>   
#define DHTTYPE DHT11                                               
#define dht_dpin 5

DHT dht(dht_dpin, DHTTYPE); 
 
char* ssid = "LMM";
char* password =  "mecatronica";
const char* mqttServer = "192.168.1.106";
const int mqttPort = 1883;
const char* mqttUser = "grupo4";
const char* mqttPassword = "grupo4_22";
const char* topic_in = "comandos";
const char* topic_out = "teste";
 
WiFiClient espClient; 
PubSubClient client(espClient);
 
void setup() {

  pinMode(22, OUTPUT);
  pinMode(23, OUTPUT);
  
  dht.begin();   
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 
  client.publish("iot/teste", "&10&400&22&37&");
  client.subscribe("iot/comandos");


  digitalWrite(22, HIGH);
  delay(2000);
  digitalWrite(22, LOW);
  digitalWrite(23, HIGH);
  delay(2000);
  digitalWrite(23, LOW);
}
 
void callback(char* topic_in, byte* message, unsigned int length) {

  String message_in;
  Serial.print("Message arrived in topic: ");
  Serial.println(topic_in);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    message_in += (char)message[i];
    
  }
  if(message_in == "manual"){
    Serial.println("entrou manual");
    digitalWrite(22, HIGH);
    digitalWrite(23, LOW);
  }
  if(message_in == "auto"){
    Serial.println("entrou auto");
    digitalWrite(23, HIGH);
    digitalWrite(22, LOW);
  }
  
  Serial.println();
  Serial.println("-----------------------");
 
}

void loop() {

  float co2 = 36.78;
  float pot = 400.58;
  float cor = 10.85;                                         
  //float t = dht.readTemperature();                                          
  float t = 25; 
  if (isnan(t)) {                                               
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  String payload = "&";
 
  payload += t; payload += "&";
  payload += co2; payload += "&";
  payload += pot; payload += "&";
  payload += cor; payload += "&";
 
  // Send payload to MQTT

  char mensagem[300];
  payload.toCharArray(mensagem, 300 );
  
  client.loop();
  client.publish("iot/teste", mensagem);
  delay(3000);
}
