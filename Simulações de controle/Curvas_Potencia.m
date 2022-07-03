%% Obtenção das curvas para diferentes temperaturas ambiente

%Dados do sistema
Tamb=31;
purple = [0.4940 0.1840 0.5560];
orange = [0.8500 0.3250 0.0980];
dark_red = [0.6350 0.0780 0.1840];

figure;
for i=1:10
    Tamb= Tamb-1;
    sim('AC_PotMed')
    color = ['r', 'b', 'g', 'y', 'k', 'm', 'c', orange, purple, dark_red];
    plot(t_sim.Data,p_med.Data,color(i))
    hold on;  
end
axis([0 1600 -0.02 0.72]);
xlabel('Tempo [s]');
ylabel('Potência média [kW]');
legend('Ta = 30', 'Ta = 29', 'Ta = 28', 'Ta = 27', 'Ta = 26', 'Ta = 25', 'Ta = 24', 'Ta = 23', 'Ta = 22', 'Ta = 21'); 
grid on;