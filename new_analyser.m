clc
clear
close all

%DANE
fp = 100e3; %F PROBKOWANIA
dt = 1/fp;

csv = readmatrix(uigetfile({'*.csv','CSV (*.CSV, )'}, 'Wybierz plik z danymi'));

data_read = csv(:,2);
time_read = csv(:,1);

max_time = max(time_read);


%TABELA CZASU UZUPELNIONA
time = (dt : dt : max_time)';
%TABELA DANYCH DO UZUPELNIENIA


figure
plot(time_read, data_read);
grid on;
ylim([-1.1*max(data_read) 1.1*max(data_read)])