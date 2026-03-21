clc
clear
close all

csv_read = readmatrix(uigetfile({'*.csv','CSV (*.CSV, )'}, 'Wybierz plik z danymi'));
fs = 100e3;
%t = 0:1/fs:2-1/fs; %do pokazu dzialania
%y = chirp(t, 100, 1, 200, 'quadratic'); %do pokazu dzialania

data_read = csv_read(:,2);
nsc = 1024;
nov = floor(nsc/2);
nff = nsc*2;



[s,f,t] = spectrogram(data_read, hamming(nsc), nov, nff, fs, 'power', 'yaxis');

figure
%spectrogram(data_read, 'yaxis');
spectrogram(data_read, hamming(nsc), nov, nff, fs, 'power', 'yaxis');
%pspectrum(data_read, 100e3, "spectrogram", 'FrequencyLimits', [9.5e3 10.5e3], 'TimeResolution', 1);
%spectrogram(y, hamming(100), 80, 100, fs, 'power', 'yaxis'); %do pokazania dzialania spektrogramu
title("Spektrogram");
xlabel("Czas [s]");
ylabel("Częstotliwość [kHz]");
ylim([1 2]);
Xlim=get(gca, 'xlim');
set(gca, 'XTick', [100, 500, 1000, 1500, 2000, 2500]);
set(gca, 'XTicklabel', [100, 500, 1000, 1500, 2000, 2500]);