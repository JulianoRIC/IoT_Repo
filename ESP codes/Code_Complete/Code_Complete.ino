//Adicionando bibliotecas
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
//#include <ESP8266WiFi.h>
//#include <ESP8266Ping.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <Time.h>
//#include<ADS1115_WE.h> 
#include<Wire.h>2
#include "DHT.h"
#include "EmonLib.h"0
#include <String.h>

EnergyMonitor emon1;
int rede = 220;
//Pino do sensor SCT
int pino_sct = 36;
int presenca = 0;

char* ssid = "LMM";
char* password =  "mecatronica";
const char* mqttServer = "192.168.1.106";
const int mqttPort = 1883;
const char* mqttUser = "grupo4";
const char* mqttPassword = "grupo4_22";
const char* topic_in = "comandos";
const char* topic_out = "dados";

WiFiClient espClient; 
PubSubClient client(espClient);


const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

uint16_t poweroff[] = {2886, 1854,  380, 1166,  354, 1192,  354, 470,  332, 494,  380, 446,  380, 1164,  380, 444,  380, 460,  382, 1164,  380, 1166,  404, 422,  380, 1164,  406, 420,  404, 420,  404, 1142,  382, 1178,  406, 420,  404, 1142,  404, 1142,  406, 418,  382, 444,  380, 1166,  382, 444,  382, 458,  380, 1164,  380, 444,  380, 444,  380, 444,  380, 444,  356, 470,  380, 444,  380, 460,  382, 442,  378, 446,  380, 444,  380, 446,  380, 444,  380, 446,  404, 420,  380, 458,  406, 418,  380, 444,  406, 1140,  382, 444,  378, 446,  380, 1164,  382, 444,  380, 458,  382, 1164,  406, 1142,  382, 444,  380, 444,  380, 444,  380, 446,  380, 446,  380, 458,  382, 444,  380, 446,  380, 444,  380, 1166,  406, 420,  378, 446,  378, 446,  380, 458,  380, 1166,  406, 418,  380, 1166,  404, 420,  404, 420,  380, 444,  380, 1164,  406, 434,  406, 420,  380, 446,  380, 444,  404, 446,  354, 444,  402, 422,  378, 470,  354, 460,  380, 448,  400, 424,  378, 470,  354, 444,  380, 446,  378, 446,  378, 470,  354, 460,  380, 448,  378, 446,  378, 448,  376, 446,  378, 470,  354, 446,  380, 446,  380, 458,  380, 470,  354, 470,  354, 470,  352, 472,  354, 470,  354, 470,  354, 470,  354, 462,  378, 1166,  380, 470,  354, 472,  354, 1168,  380, 470,  354, 472,  352, 448,  378, 1184,  378};  // UNKNOWN C8080FBE

uint16_t set_temp_16[] = {2956, 1780, 408, 1164, 380, 1108, 436, 444, 380, 444, 380, 444, 380, 1164, 380, 444, 380, 460, 380, 1164, 380, 1164, 380, 444, 380, 1136, 408, 444, 376, 448, 380, 1160, 384, 1148, 408, 444, 380, 1164, 380, 1164, 384, 444, 376, 472, 352, 1164, 384, 444, 356, 484, 376, 1164, 380, 444, 380, 444, 380, 472, 352, 444, 380, 444, 380, 448, 376, 460, 356, 468, 356, 468, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 460, 380, 444, 380, 444, 380, 1108, 436, 444, 380, 444, 380, 1160, 384, 444, 380, 456, 380, 1140, 404, 1164, 380, 444, 384, 444, 380, 440, 384, 440, 380, 448, 380, 456, 380, 1164, 380, 1108, 436, 1164, 384, 1136, 408, 440, 380, 444, 384, 440, 380, 460, 380, 1164, 380, 444, 380, 1136, 408, 444, 380, 444, 380, 444, 380, 444, 380, 460, 380, 444, 380, 440, 384, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 456, 384, 440, 384, 440, 380, 444, 384, 444, 380, 444, 380, 440, 384, 440, 384, 456, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 456, 384, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 456, 384, 440, 384, 440, 384, 440, 384, 440, 384, 1136, 408, 444, 380, 1136, 408, 456, 384 };  // UNKNOWN E06265AC

uint16_t set_temp_17[] = {2936, 1824, 360, 1128, 440, 1160, 360, 468, 356, 468, 356, 468, 380, 1164, 380, 444, 356, 484, 356, 1160, 408, 1136, 404, 448, 380, 1160, 384, 444, 356, 468, 380, 1160, 360, 1172, 412, 444, 376, 1140, 404, 1136, 388, 468, 376, 448, 376, 1164, 360, 468, 356, 480, 380, 1136, 408, 444, 356, 468, 380, 444, 356, 468, 380, 444, 356, 468, 380, 460, 356, 492, 356, 444, 356, 468, 356, 468, 380, 444, 356, 468, 380, 444, 356, 480, 384, 444, 356, 468, 356, 1160, 408, 440, 380, 448, 356, 1160, 408, 444, 380, 456, 380, 1136, 408, 1136, 388, 464, 380, 448, 376, 448, 352, 472, 356, 464, 360, 480, 356, 468, 380, 1136, 408, 1136, 384, 1164, 380, 468, 356, 468, 380, 444, 356, 484, 380, 1160, 360, 468, 380, 1160, 384, 444, 380, 444, 356, 468, 356, 468, 380, 456, 384, 440, 384, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 356, 480, 384, 440, 384, 440, 384, 440, 356, 468, 380, 444, 380, 444, 360, 464, 384, 456, 380, 444, 380, 444, 380, 444, 380, 444, 356, 468, 356, 464, 360, 468, 380, 456, 384, 444, 380, 444, 380, 444, 380, 444, 380, 440, 384, 444, 380, 444, 380, 460, 380, 1132, 412, 1132, 412, 1132, 412, 1136, 408, 440, 384, 440, 384, 1132, 412, 460, 380};  // UNKNOWN 6911692E

uint16_t set_temp_18[] = {2932, 1748, 464, 1080, 464, 1112, 432, 420, 404, 420, 404, 416, 408, 1136, 408, 420, 404, 432, 408, 1136, 404, 1112, 436, 416, 408, 1136, 404, 420, 404, 420, 404, 1084, 460, 1156, 404, 420, 404, 1112, 436, 1136, 404, 420, 404, 420, 404, 1116, 432, 416, 404, 436, 404, 1136, 408, 420, 404, 420, 404, 420, 404, 420, 404, 420, 404, 420, 380, 456, 408, 420, 404, 416, 408, 420, 404, 416, 408, 416, 408, 420, 404, 416, 408, 432, 408, 440, 384, 416, 404, 1140, 408, 416, 404, 420, 408, 1108, 436, 416, 408, 432, 404, 1112, 432, 1112, 432, 420, 404, 420, 404, 420, 404, 420, 404, 420, 408, 432, 404, 1108, 436, 420, 404, 1136, 408, 1136, 408, 444, 356, 444, 404, 420, 404, 432, 384, 1160, 408, 416, 408, 1136, 408, 416, 408, 416, 408, 416, 408, 416, 408, 432, 408, 416, 408, 416, 384, 440, 408, 416, 404, 420, 408, 416, 384, 440, 408, 432, 404, 420, 404, 416, 408, 420, 404, 444, 380, 420, 380, 468, 380, 420, 380, 456, 384, 440, 384, 440, 384, 440, 408, 444, 356, 440, 384, 440, 408, 416, 384, 456, 408, 416, 384, 440, 384, 440, 384, 440, 404, 420, 384, 440, 384, 440, 384, 456, 404, 420, 384, 1132, 412, 1156, 408, 1112, 408, 444, 384, 440, 384, 1132, 412, 456, 384}; 

uint16_t set_temp_19[] = {2940, 1800, 408, 1160, 384, 1160, 384, 444, 380, 444, 380, 444, 380, 1132, 436, 420, 380, 456, 384, 1132, 412, 1160, 384, 440, 384, 1160, 384, 440, 384, 440, 384, 1136, 408, 1148, 412, 440, 380, 1136, 412, 1132, 412, 440, 384, 440, 384, 1160, 384, 440, 384, 456, 380, 1136, 408, 444, 380, 444, 384, 440, 384, 440, 380, 444, 384, 440, 384, 456, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 404, 420, 384, 452, 384, 440, 384, 440, 384, 1160, 384, 440, 384, 440, 384, 1132, 436, 420, 404, 432, 384, 1132, 412, 1160, 384, 440, 380, 444, 384, 440, 384, 440, 384, 440, 384, 456, 380, 444, 380, 444, 380, 1160, 384, 1136, 408, 444, 380, 444, 380, 444, 384, 456, 380, 1160, 384, 444, 380, 1160, 384, 444, 380, 444, 380, 444, 380, 440, 384, 456, 384, 440, 384, 440, 384, 440, 384, 440, 384, 440, 384, 440, 384, 440, 384, 456, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 384, 440, 380, 444, 384, 456, 380, 444, 380, 440, 384, 440, 384, 444, 404, 420, 380, 444, 380, 444, 380, 456, 384, 440, 384, 440, 384, 440, 384, 444, 380, 440, 384, 440, 384, 444, 380, 456, 380, 1160, 388, 444, 376, 1136, 412, 1132, 412, 440, 384, 440, 384, 1132, 412, 456, 384 }; 

uint16_t set_temp_20[] = {2928, 1780, 408, 1136, 408, 1136, 408, 444, 380, 444, 380, 416, 408, 1136, 408, 444, 380, 456, 384, 1132, 412, 1136, 408, 444, 380, 1136, 408, 444, 380, 440, 384, 1136, 408, 1148, 412, 440, 384, 1132, 412, 1132, 412, 440, 384, 440, 384, 1136, 408, 440, 384, 456, 380, 1108, 436, 468, 356, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 456, 384, 444, 380, 444, 380, 444, 380, 416, 408, 444, 380, 440, 384, 444, 380, 456, 384, 440, 384, 440, 384, 1132, 412, 440, 384, 468, 356, 1136, 408, 440, 384, 456, 380, 1140, 404, 1136, 412, 440, 380, 444, 380, 444, 380, 444, 380, 444, 384, 456, 380, 1136, 408, 1136, 408, 444, 380, 1164, 380, 444, 380, 444, 380, 444, 380, 460, 380, 1136, 408, 444, 380, 1136, 408, 444, 380, 444, 384, 440, 380, 444, 380, 456, 384, 440, 384, 444, 380, 444, 380, 440, 384, 440, 384, 444, 380, 440, 384, 456, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 384, 456, 380, 444, 380, 444, 380, 440, 384, 444, 380, 444, 380, 444, 380, 444, 380, 460, 380, 444, 380, 468, 356, 440, 384, 440, 384, 444, 380, 444, 380, 444, 380, 456, 384, 440, 384, 440, 384, 1136, 408, 1132, 412, 440, 384, 440, 384, 1136, 408, 456, 384};

uint16_t set_temp_21[] = {2886, 1856,  380, 1140,  380, 1166,  402, 446,  356, 468,  356, 470,  356, 1190,  356, 470,  354, 460,  382, 1164,  382, 1164,  380, 470,  356, 1166,  382, 468,  332, 494,  358, 1164,  382, 1204,  356, 468,  382, 1138,  408, 1164,  382, 444,  382, 442,  382, 1138,  434, 416,  382, 456,  408, 1112,  408, 442,  408, 416,  408, 418,  408, 416,  408, 416,  408, 416,  408, 430,  408, 390,  408, 442,  408, 416,  408, 418,  408, 416,  406, 420,  406, 418,  408, 432,  384, 440,  406, 418,  384, 1136,  410, 440,  408, 418,  406, 1114,  410, 442,  384, 456,  408, 1112,  410, 1162,  384, 442,  384, 442,  382, 442,  384, 440,  384, 440,  384, 456,  384, 440,  382, 1138,  410, 440,  384, 1138,  410, 440,  382, 442,  384, 442,  382, 456,  382, 1162,  384, 442,  382, 1162,  384, 442,  382, 442,  382, 442,  384, 442,  382, 458,  382, 442,  382, 442,  382, 442,  384, 442,  382, 442,  384, 442,  384, 440,  382, 458,  382, 442,  380, 444,  380, 444,  382, 444,  382, 444,  378, 444,  380, 444,  380, 460,  380, 446,  354, 470,  356, 468,  354, 470,  354, 470,  352, 472,  354, 470,  352, 486,  354, 472,  352, 472,  352, 472,  352, 472,  352, 472,  352, 472,  352, 474,  352, 486,  354, 1192,  352, 1194,  378, 448,  378, 1166,  378, 448,  400, 424,  378, 1168,  400, 440,  378};  // UNKNOWN F471A71D

uint16_t set_temp_22[] = {2908, 1834,  382, 1164,  382, 1164,  382, 442,  380, 444,  380, 444,  380, 1136,  410, 444,  380, 458,  380, 1140,  406, 1164,  382, 444,  380, 1138,  408, 442,  380, 444,  382, 1166,  380, 1180,  382, 442,  382, 1164,  382, 1138,  408, 442,  382, 420,  406, 1138,  408, 418,  406, 432,  406, 1138,  408, 418,  406, 418,  406, 418,  382, 444,  406, 418,  406, 418,  432, 408,  382, 444,  406, 418,  432, 392,  382, 444,  432, 392,  432, 392,  458, 366,  432, 408,  408, 418,  456, 368,  432, 1114,  434, 392,  382, 442,  434, 1086,  484, 366,  382, 458,  432, 1084,  462, 1112,  434, 390,  382, 444,  408, 418,  406, 418,  406, 418,  406, 432,  406, 1140,  432, 392,  408, 416,  408, 1140,  432, 392,  408, 416,  432, 394,  406, 432,  432, 1116,  432, 392,  430, 1086,  460, 392,  432, 392,  406, 418,  408, 418,  406, 432,  408, 418,  406, 418,  408, 418,  406, 418,  406, 418,  408, 416,  408, 418,  406, 432,  408, 416,  408, 416,  408, 418,  406, 418,  408, 418,  408, 416,  408, 418,  408, 432,  408, 416,  408, 416,  408, 416,  432, 392,  408, 416,  408, 418,  408, 416,  408, 432,  408, 416,  408, 416,  408, 418,  408, 416,  406, 418,  408, 416,  408, 418,  408, 432,  408, 416,  408, 1138,  432, 392,  408, 1138,  434, 390,  408, 416,  408, 1138,  434, 408,  408};  // TEKNOPOINT

uint16_t set_temp_23[] = {2936, 1804,  384, 1164,  406, 1140,  382, 444,  382, 442,  380, 444,  382, 1164,  382, 444,  404, 432,  382, 1162,  384, 1162,  384, 444,  380, 1162,  384, 442,  382, 442,  382, 1162,  384, 1176,  384, 444,  380, 1162,  408, 1138,  408, 420,  406, 420,  404, 1140,  382, 442,  406, 432,  408, 1138,  408, 418,  406, 418,  408, 414,  408, 418,  408, 442,  384, 418,  408, 432,  408, 414,  408, 418,  406, 418,  408, 416,  408, 414,  410, 416,  408, 416,  408, 432,  408, 414,  410, 416,  410, 1078,  468, 414,  410, 416,  410, 1134,  412, 414,  410, 430,  410, 1134,  412, 1134,  412, 414,  410, 414,  410, 416,  410, 414,  410, 416,  410, 428,  408, 416,  408, 416,  408, 416,  408, 1136,  412, 414,  410, 414,  410, 416,  408, 432,  408, 1136,  410, 416,  408, 1136,  410, 418,  406, 416,  410, 418,  406, 418,  406, 432,  406, 418,  408, 420,  404, 418,  406, 420,  406, 420,  404, 420,  406, 418,  406, 432,  406, 420,  404, 420,  404, 420,  404, 422,  402, 422,  402, 422,  378, 446,  402, 434,  382, 444,  404, 422,  378, 446,  402, 420,  380, 446,  378, 446,  378, 446,  378, 458,  380, 446,  378, 446,  378, 446,  378, 470,  354, 446,  378, 448,  378, 446,  378, 460,  380, 1164,  380, 446,  380, 444,  380, 1166,  380, 446,  380, 446,  378, 1164,  382, 462,  380};  // TEKNOPOINT

uint16_t set_temp_24[] = {2956, 1804, 384, 1160, 384, 1160, 384, 440, 384, 444, 380, 440, 384, 1136, 408, 440, 384, 456, 380, 1108, 436, 1164, 384, 440, 380, 1140, 408, 440, 384, 440, 384, 1132, 412, 1176, 380, 444, 380, 1164, 380, 1136, 412, 440, 380, 444, 380, 1164, 384, 440, 384, 456, 380, 1160, 384, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 460, 380, 440, 384, 440, 384, 444, 380, 440, 384, 440, 384, 444, 380, 444, 384, 452, 384, 440, 384, 440, 384, 1136, 408, 444, 380, 440, 384, 1132, 412, 440, 384, 456, 384, 1132, 412, 1136, 408, 440, 384, 440, 384, 440, 384, 440, 384, 440, 384, 456, 380, 1136, 408, 1136, 408, 1136, 412, 440, 380, 444, 384, 440, 384, 440, 384, 456, 380, 440, 384, 1136, 412, 440, 380, 444, 380, 444, 384, 440, 380, 444, 380, 460, 380, 440, 384, 440, 384, 444, 380, 416, 408, 444, 380, 440, 384, 444, 380, 456, 384, 440, 384, 440, 384, 440, 384, 440, 384, 440, 384, 440, 384, 440, 384, 456, 380, 444, 384, 440, 380, 444, 384, 440, 384, 440, 384, 440, 384, 440, 384, 456, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 456, 384, 1132, 412, 444, 380, 1136, 408, 440, 384, 440, 384, 444, 380, 1136, 408, 460, 380};

uint16_t set_temp_25[] = {2962, 1778,  432, 1116,  406, 1136,  408, 444,  382, 442,  380, 444,  380, 1140,  406, 444,  380, 436,  404, 1140,  406, 1140,  406, 444,  380, 1140,  406, 446,  378, 446,  378, 1142,  404, 1156,  406, 446,  354, 1166,  382, 1164,  380, 470,  354, 446,  378, 1166,  380, 470,  354, 460,  380, 1164,  382, 444,  380, 446,  378, 446,  378, 444,  380, 446,  380, 470,  354, 460,  380, 470,  354, 446,  378, 470,  356, 470,  356, 444,  380, 446,  380, 444,  382, 456,  384, 440,  382, 444,  382, 1162,  382, 444,  382, 444,  382, 1162,  384, 444,  380, 456,  382, 1162,  384, 1162,  384, 442,  384, 442,  382, 442,  382, 466,  358, 442,  382, 456,  384, 442,  382, 1162,  384, 1162,  386, 442,  382, 466,  358, 442,  382, 468,  358, 482,  358, 1160,  384, 468,  358, 1106,  438, 468,  358, 468,  356, 468,  358, 468,  358, 458,  384, 466,  356, 442,  382, 442,  382, 468,  356, 446,  380, 468,  356, 468,  356, 458,  382, 444,  380, 444,  380, 444,  380, 470,  356, 468,  356, 468,  356, 468,  356, 460,  382, 468,  356, 444,  380, 446,  378, 446,  380, 470,  354, 470,  354, 470,  354, 458,  382, 468,  354, 470,  354, 470,  354, 470,  354, 472,  352, 472,  352, 472,  354, 460,  378, 1164,  380, 1168,  378, 1168,  378, 470,  352, 472,  354, 470,  352, 1166,  378, 464,  378};  // TEKNOPOINT

uint16_t set_temp_26[] = {2932, 1828, 384, 1132, 412, 1160, 384, 444, 380, 444, 356, 492, 356, 1160, 384, 440, 384, 456, 356, 1164, 408, 1136, 408, 440, 380, 1164, 384, 440, 380, 444, 384, 1136, 384, 1172, 408, 444, 380, 1136, 408, 1164, 384, 440, 380, 444, 380, 1136, 408, 444, 380, 460, 380, 1160, 384, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 356, 480, 384, 440, 384, 440, 384, 440, 384, 440, 384, 444, 380, 440, 384, 444, 380, 456, 356, 468, 384, 440, 384, 1160, 380, 444, 380, 444, 384, 1160, 384, 440, 384, 456, 380, 1136, 408, 1160, 384, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 460, 356, 1160, 408, 444, 380, 1160, 384, 444, 380, 444, 380, 444, 380, 444, 380, 456, 384, 1160, 384, 440, 384, 1132, 412, 440, 384, 440, 384, 440, 384, 440, 384, 456, 384, 440, 356, 468, 380, 444, 380, 444, 380, 444, 384, 440, 380, 444, 384, 456, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 384, 440, 380, 444, 380, 456, 384, 444, 380, 440, 384, 440, 384, 440, 384, 440, 384, 440, 384, 468, 356, 456, 384, 440, 380, 444, 384, 440, 384, 440, 384, 440, 380, 444, 380, 444, 384, 456, 384, 440, 380, 1136, 408, 1136, 408, 444, 380, 444, 380, 444, 380, 1132, 412, 460, 384};

uint16_t set_temp_27[] = {2932, 1804, 376, 1168, 376, 1168, 456, 396, 352, 472, 376, 448, 360, 1160, 432, 420, 352, 460, 380, 1140, 436, 1108, 412, 464, 380, 1140, 404, 420, 380, 444, 380, 1136, 408, 1176, 384, 444, 380, 1160, 384, 1136, 408, 444, 380, 444, 380, 1136, 408, 444, 380, 456, 384, 1132, 412, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 456, 380, 444, 380, 448, 376, 444, 380, 448, 376, 448, 376, 444, 380, 444, 380, 460, 356, 468, 356, 468, 380, 1164, 380, 444, 380, 468, 332, 1188, 380, 444, 380, 456, 384, 1160, 384, 1136, 408, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 456, 384, 440, 380, 448, 380, 1136, 408, 440, 384, 468, 356, 440, 384, 444, 380, 456, 380, 444, 380, 1136, 408, 444, 384, 440, 380, 444, 384, 440, 384, 440, 384, 456, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 456, 384, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 444, 380, 456, 384, 440, 384, 440, 380, 444, 384, 444, 380, 440, 384, 444, 380, 444, 380, 456, 384, 440, 380, 444, 380, 444, 384, 444, 380, 440, 380, 444, 380, 444, 384, 456, 380, 444, 380, 1160, 384, 444, 380, 444, 380, 444, 380, 444, 380, 1136, 408, 460, 380};

uint16_t powerOn[] =  {2938, 1784,  326, 1218,  328, 1218,  378, 444,  380, 446,  404, 420,  404, 1142,  404, 420,  404, 434,  406, 1140,  430, 1116,  404, 420,  406, 1142,  404, 418,  406, 420,  406, 1140,  406, 1154,  406, 420,  430, 1118,  428, 1116,  406, 420,  404, 420,  406, 1116,  430, 418,  406, 410,  430, 1140,  428, 396,  406, 420,  404, 396,  404, 420,  404, 444,  380, 420,  406, 434,  404, 420,  406, 444,  380, 444,  406, 396,  404, 444,  404, 396,  406, 444,  380, 434,  406, 422,  404, 420,  406, 442,  380, 444,  408, 416,  380, 1140,  432, 418,  406, 408,  430, 1116,  432, 1116,  430, 418,  406, 418,  406, 418,  406, 420,  406, 420,  406, 408,  430, 418,  406, 418,  406, 420,  406, 1140,  406, 418,  406, 418,  406, 418,  406, 432,  406, 1140,  406, 420,  406, 1140,  406, 420,  404, 420,  406, 420,  380, 1166,  406, 434,  380, 444,  406, 418,  382, 444,  404, 420,  380, 444,  380, 444,  382, 444,  380, 460,  380, 446,  380, 446,  378, 444,  380, 446,  380, 446,  378, 446,  380, 444,  378, 460,  380, 446,  404, 420,  378, 446,  380, 446,  380, 444,  378, 472,  354, 446,  378, 458,  354, 472,  352, 496,  354, 472,  354, 448,  376, 472,  328, 472,  378, 472,  352, 486,  328, 1216,  328, 498,  326, 1218,  330, 496,  328, 498,  300, 524,  328, 496,  328, 1210,  352};  // UNKNOWN 2D337BE8

#define DHTPIN 5            // pino do DHT11
#define DHTTYPE DHT11       // tipo do sensor: DHT 11
DHT dht(DHTPIN, DHTTYPE);

float temperatura_ambiente = 23;
float tempAnt;
// Sensor de presenca
int pinopir = 12; //Pino ligado ao sensor PIR

void setup() {
  irsend.begin();
  pinMode(pinopir, INPUT);
  Serial.begin(9600);
  //Pino, calibracao - Cur Const= Ratio/BurdenR. 2000/33 = 60
  emon1.current(pino_sct, 60);
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

  client.subscribe("iot/comandos");
}

void callback(char* topic_in, byte* message, unsigned int length) {

  String message_in;
  Serial.print("Message arrived in topic: ");
  Serial.println(topic_in);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    // Este if foi adicionado para desprezar mensagens contendo ruído
    if (length > 3){
      break;
    }
     
    Serial.print((char)message[i]);
    message_in += (char)message[i];
  }

/*  switch (message_in) {
  case "on":
    Serial.println("set on");
    ac_PowerOn();
    delay(500);     
    break;
  case "off":
    Serial.println("set off");
    ac_PowerOff(); 
    break;
  case "21":
    Serial.println("set 21");
    ac_setTemp21();
    break;   
  case "22":
    Serial.println("set 22");
    ac_setTemp22();
    break;    
  case "23":
    Serial.println("set 23");
    ac_setTemp23();
    break;    
   case "24":
    Serial.println("set 24");
    ac_setTemp24();
    break;    
   case "25":
    Serial.println("set 25");
    ac_setTemp25();
    break;                
  default:
    // Se nenhum dos anteriores, faz o caso padrão default
    // default é opcional
    break;
  }*/

  if(message_in == "on"){
    Serial.println("set on");
    ac_PowerOn();
    delay(500);     
  }
  if(message_in == "off"){
    Serial.println("set off");
    ac_PowerOff();
  }
    if(message_in == "16"){
    Serial.println("set 16");
    ac_setTemp16();
  }
    if(message_in == "17"){
    Serial.println("set 17");
    ac_setTemp17();
  }
      if(message_in == "18"){
    Serial.println("set 18");
    ac_setTemp18();
  }
      if(message_in == "19"){
    Serial.println("set 19");
    ac_setTemp19();
  }
      if(message_in == "20"){
    Serial.println("set 20");
    ac_setTemp20();
  }
    if(message_in == "21"){
    Serial.println("set 21");
    ac_setTemp21();
  }
    if(message_in == "22"){
    Serial.println("set 22");
    ac_setTemp22();
  }
    if(message_in == "23"){
    Serial.println("set 23");
    ac_setTemp23();
  }
    if(message_in == "24"){
    Serial.println("set 24");
    ac_setTemp24();
  }
    if(message_in == "25"){
    Serial.println("set 25");
    ac_setTemp25();
  }
    if(message_in == "26"){
    Serial.println("set 26");
    ac_setTemp26();
  }
    if(message_in == "27"){
    Serial.println("set 27");
    ac_setTemp27();
  }
  Serial.println(); 
}

void ac_PowerOn(){
    irsend.sendRaw(poweroff, sizeof(poweroff) / sizeof(poweroff[0]), 38);  // Send a raw data capture at 38kHz.
    delay(100);
    Serial.println("Comando de ligar enviado");  
}

void ac_PowerOff(){
    irsend.sendRaw(powerOn, sizeof(powerOn) / sizeof(powerOn[0]), 38);  // Send a raw data capture at 38kHz.
    delay(100);
    Serial.println("Comando de desligar enviado");  
}

void ac_setTemp16(){
    irsend.sendRaw(set_temp_16, sizeof(set_temp_16) / sizeof(set_temp_16[0]), 38);  // Send a raw data capture at 38kHz.  
    delay(100);
    Serial.println("Comando de alterar a temperatura para 16ºC enviado.");  
}

void ac_setTemp17(){
    irsend.sendRaw(set_temp_17, sizeof(set_temp_17) / sizeof(set_temp_17[0]), 38);  // Send a raw data capture at 38kHz.  
    delay(100);
    Serial.println("Comando de alterar a temperatura para 17ºC enviado.");  
}

void ac_setTemp18(){
    irsend.sendRaw(set_temp_18, sizeof(set_temp_18) / sizeof(set_temp_18[0]), 38);  // Send a raw data capture at 38kHz.  
    delay(100);
    Serial.println("Comando de alterar a temperatura para 18ºC enviado.");  
}

void ac_setTemp19(){
    irsend.sendRaw(set_temp_19, sizeof(set_temp_19) / sizeof(set_temp_19[0]), 38);  // Send a raw data capture at 38kHz.  
    delay(100);
    Serial.println("Comando de alterar a temperatura para 19ºC enviado.");  
}

void ac_setTemp20(){
    irsend.sendRaw(set_temp_20, sizeof(set_temp_20) / sizeof(set_temp_20[0]), 38);  // Send a raw data capture at 38kHz.  
    delay(100);
    Serial.println("Comando de alterar a temperatura para 20ºC enviado.");  
}
void ac_setTemp21(){
    irsend.sendRaw(set_temp_21, sizeof(set_temp_21) / sizeof(set_temp_21[0]), 38);  // Send a raw data capture at 38kHz.  
    delay(100);
    Serial.println("Comando de alterar a temperatura para 21ºC enviado.");  
}

void ac_setTemp22(){
    irsend.sendRaw(set_temp_22, sizeof(set_temp_22) / sizeof(set_temp_22[0]), 38);  // Send a raw data capture at 38kHz.  
    delay(100);
    Serial.println("Comando de alterar a temperatura para 22ºC enviado.");  
}

void ac_setTemp23(){
    irsend.sendRaw(set_temp_23, sizeof(set_temp_23) / sizeof(set_temp_23[0]), 38);  // Send a raw data capture at 38kHz.  
    delay(100);
    Serial.println("Comando de alterar a temperatura para 23ºC enviado.");  
}

void ac_setTemp24(){
    irsend.sendRaw(set_temp_24, sizeof(set_temp_24) / sizeof(set_temp_24[0]), 38);  // Send a raw data capture at 38kHz.  
    delay(100);
    Serial.println("Comando de alterar a temperatura para 24ºC enviado.");  
}

void ac_setTemp25(){
    irsend.sendRaw(set_temp_25, sizeof(set_temp_25) / sizeof(set_temp_25[0]), 38);  // Send a raw data capture at 38kHz.  
    delay(100);
    Serial.println("Comando de alterar a temperatura para 25ºC enviado.");  
}

void ac_setTemp26(){
    irsend.sendRaw(set_temp_26, sizeof(set_temp_26) / sizeof(set_temp_26[0]), 38);  // Send a raw data capture at 38kHz.  
    delay(100);
    Serial.println("Comando de alterar a temperatura para 26ºC enviado.");  
}

void ac_setTemp27(){
    irsend.sendRaw(set_temp_27, sizeof(set_temp_27) / sizeof(set_temp_27[0]), 38);  // Send a raw data capture at 38kHz.  
    delay(100);
    Serial.println("Comando de alterar a temperatura para 27ºC enviado.");  
}

float pega_temperatura()
{
  float temp =  dht.readTemperature();
  tempAnt = temperatura_ambiente; 

  if (isnan(temp) || (temp < 0) || (temp > 50))
  {
    Serial.println("Falha ao ler o DHT");
    temperatura_ambiente = tempAnt;
  }
  else
  {
    if ((abs(tempAnt - temp))< 5){
      temperatura_ambiente = temp;
    }
  }
  return temperatura_ambiente; 
}


void loop() {

  presenca = digitalRead(pinopir);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
   while (!client.connected()) {
    Serial.println("Connecting to MQTT..."); 
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) { 
      Serial.println("connected"); 
      client.subscribe("iot/comandos");  
    } else { 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000); 
    }
  }
  
 //Calcula a corrente
  double Irms = emon1.calcIrms(1480)/10;
  float co2 = random(3400, 3700)/100;
  float cor = 0;

  if (((Irms > 1.3)&&(Irms < 15))){
    cor = Irms;
    }

  float pot = cor*rede;                                                                              
  float t = pega_temperatura(); 
  Serial.println(t);
  String payload = "&";
 
  payload += t; payload += "&";
  payload += presenca; payload += "&";
  payload += pot; payload += "&";
  payload += cor; payload += "&";
 
  // Send payload to MQTT

  char mensagem[300];
  payload.toCharArray(mensagem, 300 );
  
  client.loop();
  client.publish("iot/dados", mensagem);
  delay(3000);
}
