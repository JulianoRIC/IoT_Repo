from datetime import datetime
from dotenv import load_dotenv, main
import os
import influxdb_client
from influxdb_client import InfluxDBClient, Point, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS
import paho.mqtt.client as mqtt

#variaveis para calculo potencia consumida
val = []
count = 0
soma = 0
pot_med = 0

#Credenciais 
bucket = "monitor"
org = "UFSC"
token = "YcKmrqYysnzjnDdPdezKeScPdDvdluoQW2_b5xcMcx3WlKIiqnCaywlpPryATj3qIqdzQaekg8WHAUYDtNup7w=="
url = "http://192.168.1.106:8086"


#instanciando cliente Influx
cliente = influxdb_client.InfluxDBClient(url = url, token = token, org = org)


write_api = cliente.write_api(write_options=SYNCHRONOUS)


################### Parte do Subscriber do Broker #######################################################

def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    # subscribe, which need to put into on_connect
    # if reconnect after losing the connection with the broker, it will continue to subscribe to the raspberry/topic topic
    client.subscribe("iot/dados")

#calculo potencia media consumida 
def calcula_pot(pot):
    global pot_med
    global soma
    global count
    global val
    val.append(pot)
    if count < 720:
        for x in val:
            soma += float(x)
        pot_med = soma/len(val)
    else:
        val.pop(0)
        for x in val:
            soma = soma + float(x)
        pot_med = soma/720
    soma = 0
    print("A potência média é: ", pot_med)
    count += 1
    return pot_med


# the callback function, it will be triggered when receiving messages
# A mensagem enviada pelo publisher deve obedecer o formato 10&400&22&37

def on_message(client, userdata, msg): #, ppayload):
    rows = []
    print(f"{msg.topic} {msg.payload}")

    ppayload = (f"{msg.payload}")
    row = ppayload.split("&")
    temperatura, presenca, potencia, corrente  = row[1], row[2], row[3], row[4]

    print("Corrente: \n", corrente)
    print("Potencia: \n", potencia)
    print("Temperatura: \n", temperatura)
    print("Presenca: \n", presenca)
    t = influxdb_client.Point("Sensores").tag("DAS", "LMM").field("temperatura", temperatura)
    p = influxdb_client.Point("Sensores").tag("DAS", "LMM").field("potencia", potencia)
    c = influxdb_client.Point("Sensores").tag("DAS", "LMM").field("corrente", corrente)
    o = influxdb_client.Point("Sensores").tag("DAS", "LMM").field("potencia_media", str(calcula_pot(potencia)))

    #print("P eh", p)
    write_api.write(bucket=bucket,org=org, record=t)
    write_api.write(bucket=bucket,org=org, record=p)     
    write_api.write(bucket=bucket,org=org, record=c)
    write_api.write(bucket=bucket,org=org, record=o)   




################### Parte do Subscriber do Broker #######################################################
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.username_pw_set("grupo4", "grupo4_22")

# set the will message, when the Raspberry Pi is powered off, or the network is interrupted abnormally, it will send the will message to other clients
#client.will_set('raspberry/status', b'{"status": "Off"}')

# create connection, the three parameters are broker address, broker port number, and keep-alive time respectively
#client.connect("broker.emqx.io", 1883, 60)
client.connect("192.168.1.106", 1883, 60)

# set the network loop blocking, it will not actively end the program before calling disconnect() or the program crash
client.loop_forever()
