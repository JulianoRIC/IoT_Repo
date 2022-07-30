from flask import Flask, flash, render_template, request, url_for, redirect, flash
import os
import paho.mqtt.client as mqtt
import time
import webbrowser

def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")

app = Flask(__name__, static_url_path='/static')
app.static_folder = 'static'
app.config['SESSION_COOKIE_SECURE'] = False


#conectando com o broker MQTT
client = mqtt.Client()
client.on_connect = on_connect
client.connect("192.168.1.106", 1883,60)
client.username_pw_set("grupo4", "grupo4_22")

@app.route("/home", methods=["GET", "POST"])

def home():

    return render_template("indexLogin.html")
    
@app.route("/login", methods=["GET", "POST"])

def login():
    if request.form.get('usuario') == "admin" and request.form.get('senha') == "admin" :
        return render_template("index.html")
    else:
        return render_template("indexLogin.html")

@app.route("/man", methods=["GET", "POST"])

def man():
    
    client.on_connect = on_connect
    client.connect("192.168.1.106", 1883,60)
    client.username_pw_set("grupo4", "grupo4_22")
    temp = request.form.get('TEMP')
    print(temp)
    client.publish('www/comandos', payload= temp, qos=0, retain=False)
    return render_template("index.html", temp = temp)
            


@app.route("/auto", methods=["GET", "POST"])

def auto():    
    potencia = request.form.get('POT')
    print("Modo Automático")
    print(potencia)
    client.on_connect = on_connect
    
    client.connect("192.168.1.106", 1883,60)
    client.username_pw_set("grupo4", "grupo4_22")
    client.publish('www/potencia', payload= "&"+potencia+"&", qos=0, retain=False) 
    print(f"Mandei  {potencia} para o broker")
    return render_template("index.html", pot = potencia)
            
@app.route("/on", methods=["GET", "POST"])

def on():    
    print("Liga enviado")
    client.on_connect = on_connect    
    client.connect("192.168.1.106", 1883,60)
    client.username_pw_set("grupo4", "grupo4_22")
    client.publish('iot/comandos', payload= "on", qos=0, retain=False) 
    return render_template("index.html")

@app.route("/off", methods=["GET", "POST"])

def off():    
    print("Desliga enviado")
    client.on_connect = on_connect    
    client.connect("192.168.1.106", 1883,60)
    client.username_pw_set("grupo4", "grupo4_22")
    client.publish('iot/comandos', payload= "off", qos=0, retain=False) 
    return render_template("index.html")

    
time.sleep(0.5)
webbrowser.open('http://127.0.0.1:5000/home')  # Go to IOT GUI   

#if __name__ == '__main__':

app.run(debug=True)

client.loop_forever()


