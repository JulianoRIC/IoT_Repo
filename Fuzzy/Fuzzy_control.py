import matplotlib.pyplot as plt
import numpy as np
import skfuzzy as fuzz
from skfuzzy import control as ctrl
import paho.mqtt.client as mqtt
import time
import random
#plt.rcParams["figure.figsize"] = (15, 6)

power = 0
temperatura = 20

"""
out_ref -> Referência de temperatura da sala
in_put -> Potência disponível para o aparelho
in_tamb -> Temperatura interna da sala -- // Precisa ser revisto porque conforme a temperatura da sala cair, a referência tende a aumentar, 
gerando uma oscilação - creio que usar a temperatura ambiente externa possa ser mais adequado//

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

# in_pot.view()
# plt.show()

in_tamb = ctrl.Antecedent(np.arange(6, 38, 0.2), 'in_tamb')

in_tamb['ITIMF'] = fuzz.gaussmf(in_tamb.universe, 6, 1.2)
in_tamb['ITIF'] = fuzz.gaussmf(in_tamb.universe, 12.96, 1.5)
in_tamb['ITIA'] = fuzz.gaussmf(in_tamb.universe, 20.12, 1.5)
in_tamb['ITIQ'] = fuzz.gaussmf(in_tamb.universe, 25.72, 1.2)
in_tamb['ITIMQ'] = fuzz.gaussmf(in_tamb.universe, 38, 3.5)

# in_tamb.view()
# plt.show()

out_ref = ctrl.Consequent(np.arange(16, 31, 0.2), 'out_ref')

out_ref['OTRCMA'] = fuzz.gaussmf(out_ref.universe, 16, 1.1)
out_ref['OTRCA'] = fuzz.gaussmf(out_ref.universe, 20, 0.75)
out_ref['OTRCM'] = fuzz.gaussmf(out_ref.universe, 23, 0.75)
out_ref['OTRCB'] = fuzz.gaussmf(out_ref.universe, 26, 0.75)
out_ref['OTRCMB'] = fuzz.gaussmf(out_ref.universe, 31, 1.3)

# out_ref.view()
# plt.show()

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
#rule26 = ctrl.Rule(in_pot['IPDB'], out_ref['OTRCMB'])
rule27 = ctrl.Rule(in_pot['IPDMB'], out_ref['OTRCMB'])
#rule28 = ctrl.Rule(in_pot['IPDM'], out_ref['OTRCB'])

# rule29 = ctrl.Rule(in_pot['IPDMB'] | in_tamb['ITIMF'], out_ref['OTRCMB']) Exemplo de regra usando associação tipo "ou"

agreg = ctrl.ControlSystem([rule1, rule2, rule3, rule4, rule5, rule6, rule7, rule8, rule9, rule10, rule11, rule12, rule13,
                            rule14, rule15, rule16, rule17, rule18, rule19, rule20, rule21, rule22, rule23, rule24, rule25, rule27])

controle = ctrl.ControlSystemSimulation(agreg)


def fuzzy_control(temp, pot):
    controle.input['in_pot'] = pot
    controle.input['in_tamb'] = temp
    controle.compute()

    ref = (controle.output['out_ref'])

    # out_ref.view(sim=controle)

    # plt.show()

    return ref


def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    client.subscribe([("iot/teste", 0), ("iot/potencia", 0)])


'''
def on_message(client, userdata, msg):  # , ppayload):
    rows = []
    print(f"{msg.topic} {msg.payload}")

    ppayload = (f"{msg.payload}")
    row = ppayload.split("&")
    temperatura = row[1]
    reff = int(fuzzy_control(float(temperatura), power))
    client.publish('iot/comandos', payload=(reff), qos=0, retain=False)
    print(f"send {reff} to iot/teste")
    time.sleep(0.2)
'''

old_reff = 0


def on_message(client, userdata, msg):
    global temperatura
    global power
    global old_reff

    if msg.topic == "iot/teste":
        # update temperatura
        print(f"{msg.topic} {msg.payload}")
        ppayload = (f"{msg.payload}")
        row = ppayload.split("&")
        temperatura = row[1]
        print("valor da temperatura medida: ", temperatura)

        reff = int(fuzzy_control(float(temperatura), float(power)))
        #ref_artificial = random.randint(21,25)
        if reff != old_reff:
            if reff >= 28:
                client.publish('iot/comandos', payload="off", qos=0, retain=False)
                old_reff = reff
                print(f"send turn off the air conditioner to iot/teste")
            else:   
                client.publish('iot/comandos', payload=(reff), qos=0, retain=False)
                old_reff = reff
                print(f"send real reference {reff} to iot/teste")
        else:
            pass
        #print(f"send real reference {reff} to iot/teste")
        #print(f"send artificial reference {ref_artificial} to iot/teste")

    if msg.topic == "iot/potencia":
        # update potencia
        print(f"{msg.topic} {msg.payload}")
        p_payload = (f"{msg.payload}")
        row = p_payload.split("&")
        power = row[1]
        print("valor da potencia setada pelo sistema: ", power)
        reff = int(fuzzy_control(float(temperatura), float(power)))
        #ref_artificial = random.randint(21,25)
        ''''
    if reff != old_reff:
        client.publish('iot/comandos', payload=(reff), qos=0, retain=False)
        old_reff = reff
    else:
        pass
    #print(f"send artificial reference {ref_artificial} to iot/teste")
    ''' 
        if reff >= 28:
            client.publish('iot/comandos', payload="off", qos=0, retain=False)
            print(f"send turn off the air conditioner to iot/teste")
        else:   
            client.publish('iot/comandos', payload=(reff), qos=0, retain=False)
            print(f"send real reference {reff} to iot/teste")


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.username_pw_set("grupo4", "grupo4_22")

client.connect("192.168.1.106", 1883, 60)

client.loop_forever()
