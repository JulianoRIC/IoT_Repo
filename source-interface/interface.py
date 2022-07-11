from flask import Flask, render_template, request, url_for, redirect
import os
import paho.mqtt.client as mqtt
import time
import webbrowser


def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")


#conectando com o broker MQTT
client = mqtt.Client()
client.on_connect = on_connect
client.connect("192.168.1.106", 1883,60)
client.username_pw_set("grupo4", "grupo4_22")

app = Flask(__name__)


@app.route("/home", methods=["GET", "POST"])

def home():
    if request.method == "POST":
        print("Button was pressed ")

        temp = request.form['TEMP']
        print(temp)
        pot = request.form['POT']

        if request.form['btn_on'] == "Manual":
            client.on_connect = on_connect
            client.connect("192.168.1.106", 1883,60)
            client.username_pw_set("grupo4", "grupo4_22")
            print("Modo Manual")
            client.publish('iot/comandos', payload="manual", qos=0, retain=False)
            return render_template("index.html")
            

        if request.form['btn_on'] == "Automático":
            print("Modo Automático")
            client.on_connect = on_connect
            client.connect("192.168.1.106", 1883,60)
            client.username_pw_set("grupo4", "grupo4_22")
            client.publish('iot/comandos', payload="auto", qos=0, retain=False) 
            return render_template("index.html")


    
    return render_template("index.html")

    
time.sleep(0.5)
webbrowser.open('http://127.0.0.1:5000/home')  # Go to IOT GUI   

#if __name__ == '__main__':

app.run(debug=True)

client.loop_forever()


