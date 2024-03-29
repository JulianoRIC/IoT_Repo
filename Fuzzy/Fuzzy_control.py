#Autores:
#Juliano Rics e Valdecir Hoffmann
#Codigo para calcular e enviar ao broker o valor da temperatura de referencia do ar-condicionado a partir das regras fuzzy

import matplotlib.pyplot as plt
import numpy as np
import skfuzzy as fuzz
from skfuzzy import control as ctrl
import paho.mqtt.client as mqtt
import time
import random
import requests
import json

#plt.rcParams["figure.figsize"] = (15, 6)

#inicializacao das variaveis (potencia disponível e temperatura interna)
power = 0
temperatura = 20

"""
out_ref -> Referência de temperatura da sala
in_put -> Potência disponível para o aparelho
in_tamb -> Temperatura interna da sala 

IPDMB -> Input de potência muito baixa
IPDB -> Input de potência baixa
IPDM -> Input de potência média
IPDA -> Input de potência alta
IPDMA -> Input de potência muito alta

ITIMF -> Input de temperatura muito fria
ITIF -> Input de temperatura fria
ITIA -> Input de temperatura agradável
ITIQ -> Input de temperatura quente
ITIMQ -> Input de temperatura muito quente

ITEMF -> Input de temperatura ext muito fria
ITEF -> Input de temperatura ext fria
ITEA -> Input de temperatura ext agradável
ITEQ -> Input de temperatura ext quente
ITEMQ -> Input de temperatura ext muito quente

OTRCMB -> Output de temperatura de referência muito baixa
OTRCB -> Output de temperatura de referência baixa
OTRCM -> Output de temperatura de referência média
OTRCA -> Output de temperatura de referência alta
OTRCMA -> Output de temperatura de referência muito alta
"""

in_pot = ctrl.Antecedent(np.arange(0, 1027, 0.2), 'in_pot')

in_pot['IPDMB'] = fuzz.gaussmf(in_pot.universe, 0, 80)
in_pot['IPDB'] = fuzz.gaussmf(in_pot.universe, 300, 70)
in_pot['IPDM'] = fuzz.gaussmf(in_pot.universe, 530, 70)
in_pot['IPDA'] = fuzz.gaussmf(in_pot.universe, 761, 60)
in_pot['IPDMA'] = fuzz.gaussmf(in_pot.universe, 1027, 50)

# ver grafico de distribuição da variavel linguistica de entrada (potencia disponivel)
# in_pot.view()
# plt.show()

in_tamb = ctrl.Antecedent(np.arange(6, 38, 0.2), 'in_tamb')

in_tamb['ITIMF'] = fuzz.gaussmf(in_tamb.universe, 6, 1.2)
in_tamb['ITIF'] = fuzz.gaussmf(in_tamb.universe, 12.96, 1.5)
in_tamb['ITIA'] = fuzz.gaussmf(in_tamb.universe, 20.12, 1.5)
in_tamb['ITIQ'] = fuzz.gaussmf(in_tamb.universe, 25.72, 1.2)
in_tamb['ITIMQ'] = fuzz.gaussmf(in_tamb.universe, 38, 3.5)

# ver grafico de distribuição da variavel linguistica de entrada (temperatura interna da sala)
# in_tamb.view()
# plt.show()

in_text = ctrl.Antecedent(np.arange(5, 40, 0.2), 'in_text')

in_text['ITEMF'] = fuzz.gaussmf(in_text.universe, 5, 1.2)
in_text['ITEF'] = fuzz.gaussmf(in_text.universe, 12, 1.5)
in_text['ITEA'] = fuzz.gaussmf(in_text.universe, 20, 1.5)
in_text['ITEQ'] = fuzz.gaussmf(in_text.universe, 27, 1.2)
in_text['ITEMQ'] = fuzz.gaussmf(in_text.universe, 40, 3.5)

# ver grafico de distribuição da variavel linguistica de entrada (temperatura externa da sala)
#in_text.view()
# plt.show()


out_ref = ctrl.Consequent(np.arange(16, 31, 0.2), 'out_ref')

out_ref['OTRCMA'] = fuzz.gaussmf(out_ref.universe, 16, 1.1)
out_ref['OTRCA'] = fuzz.gaussmf(out_ref.universe, 20, 0.75)
out_ref['OTRCM'] = fuzz.gaussmf(out_ref.universe, 23, 0.75)
out_ref['OTRCB'] = fuzz.gaussmf(out_ref.universe, 26, 0.75)
out_ref['OTRCMB'] = fuzz.gaussmf(out_ref.universe, 31, 1.3)

# ver grafico de distribuição da variavel linguistica de saida (temperatura de referencia do ar-condicionado)
# out_ref.view()
# plt.show()

#Definicao das Regras Fuzzy
rule1 = ctrl.Rule(in_pot['IPDMA'] & in_tamb['ITIMQ'], out_ref['OTRCMA'])
rule2 = ctrl.Rule(in_pot['IPDMA'] & in_tamb['ITIQ'], out_ref['OTRCA'])
rule3 = ctrl.Rule(in_pot['IPDMA'] & in_tamb['ITIA'], out_ref['OTRCM'])
rule4 = ctrl.Rule(in_pot['IPDMA'] & in_tamb['ITIF'], out_ref['OTRCB'])
rule5 = ctrl.Rule(in_pot['IPDMA'] & in_tamb['ITIMF'], out_ref['OTRCMB'])
rule6 = ctrl.Rule(in_pot['IPDA'] & in_tamb['ITIMQ'], out_ref['OTRCA'])
rule7 = ctrl.Rule(in_pot['IPDA'] & in_tamb['ITIQ'], out_ref['OTRCA'])
rule8 = ctrl.Rule(in_pot['IPDA'] & in_tamb['ITIA'], out_ref['OTRCM'])
rule9 = ctrl.Rule(in_pot['IPDA'] & in_tamb['ITIF'], out_ref['OTRCB'])
rule10 = ctrl.Rule(in_pot['IPDA'] & in_tamb['ITIMF'], out_ref['OTRCMB'])
rule11 = ctrl.Rule(in_pot['IPDM'] & in_tamb['ITIMQ'], out_ref['OTRCB'])
rule12 = ctrl.Rule(in_pot['IPDM'] & in_tamb['ITIQ'], out_ref['OTRCB'])
rule13 = ctrl.Rule(in_pot['IPDM'] & in_tamb['ITIA'], out_ref['OTRCMB'])
rule14 = ctrl.Rule(in_pot['IPDM'] & in_tamb['ITIF'], out_ref['OTRCMB'])
rule15 = ctrl.Rule(in_pot['IPDM'] & in_tamb['ITIMF'], out_ref['OTRCMB'])
rule16 = ctrl.Rule(in_pot['IPDB'] & in_tamb['ITIMQ'], out_ref['OTRCB'])
rule17 = ctrl.Rule(in_pot['IPDB'] & in_tamb['ITIQ'], out_ref['OTRCB'])
rule18 = ctrl.Rule(in_pot['IPDB'] & in_tamb['ITIA'], out_ref['OTRCMB'])
rule19 = ctrl.Rule(in_pot['IPDB'] & in_tamb['ITIF'], out_ref['OTRCMB'])
rule20 = ctrl.Rule(in_pot['IPDB'] & in_tamb['ITIMF'], out_ref['OTRCMB'])
rule21 = ctrl.Rule(in_pot['IPDMB'] & in_tamb['ITIMQ'], out_ref['OTRCB'])
rule22 = ctrl.Rule(in_pot['IPDMB'] & in_tamb['ITIQ'], out_ref['OTRCMB'])
rule23 = ctrl.Rule(in_pot['IPDMB'] & in_tamb['ITIA'], out_ref['OTRCMB'])
rule24 = ctrl.Rule(in_pot['IPDMB'] & in_tamb['ITIF'], out_ref['OTRCMB'])
rule25 = ctrl.Rule(in_pot['IPDMB'] & in_tamb['ITIMF'], out_ref['OTRCMB'])
rule26 = ctrl.Rule(in_pot['IPDMB'], out_ref['OTRCMB'])
rule27 = ctrl.Rule(in_pot['IPDMA'] & in_tamb['ITIQ'] & in_text['ITEMQ'], out_ref['OTRCMA'])
rule28 = ctrl.Rule(in_pot['IPDMA'] & in_tamb['ITIA'] & in_text['ITEMQ'], out_ref['OTRCA'])
rule29 = ctrl.Rule(in_text['ITEMF'] | in_text['ITEF'], out_ref['OTRCMB'])

#fuzzification
agreg = ctrl.ControlSystem([rule1, rule2, rule3, rule4, rule5, rule6, rule7, rule8, rule9, rule10, rule11, rule12, rule13,
                            rule14, rule15, rule16, rule17, rule18, rule19, rule20, rule21, rule22, rule23, rule24, rule25, rule26, rule27, rule28,rule29])

controle = ctrl.ControlSystemSimulation(agreg)

#Funcao para extrair valor de temperatura externa (da cidade de Floripa) a partir de uma API 
def get_temp_from_api():
    try:
        response = requests.get("https://api.hgbrasil.com/weather?woeid=90200648")
        a = (response.json())
        b = int((a['results']).get('temp'))
        print(b)
    except:
        print("An exception occurred")
        pass
    return b

#funcao para calcula saida de controle (defuzzification)
def fuzzy_control(temp, pot):

    controle.input['in_pot'] = pot
    controle.input['in_tamb'] = temp
    controle.input['in_text'] = get_temp_from_api()
    controle.compute()

    ref = (controle.output['out_ref'])

    #visualizacao do valor de saida calculado
    # out_ref.view(sim=controle)
    # plt.show()

    return ref

#conexao com o broker e inscricao nos topicos de interesse 
def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    client.subscribe([("iot/dados", 0), ("iot/potencia", 0)])

#inicializando variaveis auxiliares
old_reff = 0
presenca = 0
status_tempo = 0
inicio = time.time()  

#funcao callback para recebimento e tratamento das mensagens dos topicos do broker
def on_message(client, userdata, msg):
    global temperatura
    global power
    global old_reff
    global presenca
    global inicio
    global status_tempo

    #se a msg for um valor novo de temperatura e detectou presenca
    if msg.topic == "iot/dados":
        # update temperatura
        print(f"{msg.topic} {msg.payload}")
        ppayload = (f"{msg.payload}")
        row = ppayload.split("&")
        temperatura = row[1]
        presenca = row[2]
        print("valor da temperatura medida: ", temperatura)
        if presenca == "1":
           inicio = time.time()  #inicializa contagem de tempo
           status_tempo = 0
        fim = time.time()
        #se passou cinco minutos desde a ultima deteccao de presenca, desliga ar-condicionado
        if  fim - inicio >= 300 and status_tempo == 0:
            client.publish('iot/comandos', payload="off", qos=0, retain=False)
            time.sleep(0.1)
            print(f"send turn off the air conditioner to iot/dados")
            status_tempo = 1      
        reff = int(fuzzy_control(float(temperatura), float(power))) #valor de temperatura de referencia calculado pelo controle fuzzy
        #se a temperatura de referencia calculada eh a diferente da anterior
        if reff != old_reff:
            #se for um valor maior que 27°C --> desliga ar-condicionado
            if reff >= 28:
                client.publish('iot/comandos', payload="off", qos=0, retain=False)
                old_reff = reff
                print(f"send turn off the air conditioner to iot/dados")
                time.sleep(0.1)
            #senao manda o valor calculado mesmo
            else:   
                client.publish('iot/comandos', payload=(reff), qos=0, retain=False)
                time.sleep(0.1)
                old_reff = reff
                print(f"send real reference {reff} to iot/dados")
        else:
            pass
        
    #se um novo valor de referência de potencia disponivel for recebido
    if msg.topic == "iot/potencia":
        # update potencia
        print(f"{msg.topic} {msg.payload}")
        p_payload = (f"{msg.payload}")
        row = p_payload.split("&")
        power = row[1]
        print("valor da potencia setada pelo sistema: ", power)
        reff = int(fuzzy_control(float(temperatura), float(power)))
        #se for um valor maior que 27°C --> desliga ar-condicionado
        if reff >= 28:
            client.publish('iot/comandos', payload="off", qos=0, retain=False)
            time.sleep(0.1)
            print(f"send turn off the air conditioner to iot/dados")
        #senao manda o valor calculado mesmo
        else:   
            client.publish('iot/comandos', payload=(reff), qos=0, retain=False)
            print(f"send real reference {reff} to iot/dados")
            time.sleep(0.1)

#passa credenciais para acessar broker e se mantem conectado
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.username_pw_set("grupo4", "grupo4_22")

client.connect("192.168.1.106", 1883, 60)

client.loop_forever()
