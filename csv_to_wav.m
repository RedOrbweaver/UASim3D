clc
clear
close all

csv_read = readmatrix(uigetfile({'*.csv','CSV (*.CSV, )'}, 'Wybierz plik z danymi'));

data_read = csv_read(:,2);
fp = 100e3;
dt = 1/fp;
time = csv_read(:,1); %czas (probki)

%fft
X = fft(data_read);
X = X/max(X);

N = length(data_read);

figure
plot((fp*fp)/N*time, abs(X)); %aby wyszlo prawidlowe fft
title('FFT sygnału');
xlim([0 20000]);
ylim([0 1.1*max(abs(X))])
ylabel('|Y(f)|');
xlabel('Czestotliwosc [Hz]');
grid on;
grid minor;


figure
plot(time, data_read);
title('Podgląd sygnału');
grid on;
xlabel('Czas [s]');
ylabel('Wartość próbki');
ylim([-1.1*max(data_read) 1.1*max(data_read)]);

%sound(data_read, fp);