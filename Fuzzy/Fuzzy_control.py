import matplotlib.pyplot as plt
import numpy as np
import skfuzzy as fuzz
from skfuzzy import control as ctrl
plt.rcParams["figure.figsize"] = (15, 6)

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

in_pot['IPDMB'] = fuzz.gaussmf(in_pot.universe, 0, 109)
in_pot['IPDB'] = fuzz.gaussmf(in_pot.universe, 300, 80)
in_pot['IPDM'] = fuzz.gaussmf(in_pot.universe, 600, 80)
in_pot['IPDA'] = fuzz.gaussmf(in_pot.universe, 781, 80)
in_pot['IPDMA'] = fuzz.gaussmf(in_pot.universe, 1027, 80)

in_pot.view()
# plt.show()

in_tamb = ctrl.Antecedent(np.arange(6, 38, 0.2), 'in_tamb')

in_tamb['ITIMF'] = fuzz.gaussmf(in_tamb.universe, 6, 1.6)
in_tamb['ITIF'] = fuzz.gaussmf(in_tamb.universe, 13, 2.35)
in_tamb['ITIA'] = fuzz.gaussmf(in_tamb.universe, 20, 2.15)
in_tamb['ITIQ'] = fuzz.gaussmf(in_tamb.universe, 25.75, 2)
in_tamb['ITIMQ'] = fuzz.gaussmf(in_tamb.universe, 38, 5.7)

in_tamb.view()
# plt.show()

out_ref = ctrl.Consequent(np.arange(16, 31, 0.2), 'out_ref')

out_ref['OTRCMA'] = fuzz.gaussmf(out_ref.universe, 16, 1.1)
out_ref['OTRCA'] = fuzz.gaussmf(out_ref.universe, 20, 1)
out_ref['OTRCM'] = fuzz.gaussmf(out_ref.universe, 23, 1)
out_ref['OTRCB'] = fuzz.gaussmf(out_ref.universe, 26, 1)
out_ref['OTRCMB'] = fuzz.gaussmf(out_ref.universe, 31, 1.5)

out_ref.view()
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
rule11 = ctrl.Rule(in_pot['IPDM'] & in_tamb['ITIMQ'], out_ref['OTRCMB'])
rule12 = ctrl.Rule(in_pot['IPDM'] & in_tamb['ITIQ'], out_ref['OTRCMB'])
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
rule26 = ctrl.Rule(in_pot['IPDMA'], out_ref['OTRCMB'])
rule27 = ctrl.Rule(in_pot['IPDMA'], out_ref['OTRCMB'])
rule28 = ctrl.Rule(in_pot['IPDMA'], out_ref['OTRCMB'])


# rule29 = ctrl.Rule(in_pot['IPDMB'] | in_tamb['ITIMF'], out_ref['OTRCMB']) Exemplo de regra usando associação tipo "ou"

agreg = ctrl.ControlSystem([rule1, rule2, rule3, rule4, rule5, rule6, rule7, rule8, rule9, rule10, rule11, rule12, rule13,
                           rule14, rule15, rule16, rule17, rule18, rule19, rule20, rule21, rule22, rule23, rule24, rule25, rule26, rule27, rule28])

controle = ctrl.ControlSystemSimulation(agreg)

controle.input['in_pot'] = 300
controle.input['in_tamb'] = 29
controle.compute()

print(controle.output['out_ref'])

out_ref.view(sim=controle)

plt.show()
