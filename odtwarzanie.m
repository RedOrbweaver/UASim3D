% analyze_wav_fft.m
% Wczytuje plik WAV, rysuje przebieg w czasie (słupkowo) i widmo FFT.

clear
clc
close all

%plik
[fn, fp] = uigetfile({'*.wav','WAV files (*.wav)'}, 'Wybierz plik WAV');
if isequal(fn,0)
    error('Przerwano wybór pliku.');
end
file = fullfile(fp, fn);

%wczytywanie danych
[x, fs] = audioread(file);        
info    = audioinfo(file);
N       = size(x,1);
C       = size(x,2);

fprintf('Plik: %s\nfs: %d Hz | kanały: %d | bit depth: %d | długość: %.2f s\n', ...
    fn, fs, C, info.BitsPerSample, N/fs);

x_ch = x(:, min(chan, C));
t = (0:N-1).'/fs;

%ograniczenie wykresow
maxBars = 8000;
step    = max(1, ceil(N/maxBars));
tb      = t(1:step:end);
xb      = x_ch(1:step:end);

%fft
%okno hanna
w  = 0.5 - 0.5*cos(2*pi*(0:N-1)'/(N-1));
xw = x_ch .* w;

Nfft = 2^nextpow2(N);             %zero-padding do potęgi 2
X    = fft(xw, Nfft);
P2   = abs(X/N);                  
P1   = P2(1:Nfft/2+1);             
P1(2:end-1) = 2*P1(2:end-1);      
fax  = fs*(0:(Nfft/2))/Nfft;

%wykresy
figure('Name','Analiza WAV','Color','w');
tiledlayout(2,1,'TileSpacing','compact');

% 1) Czas
nexttile;
%stem(tb, xb, 'Marker','none');
plot(tb, xb);
grid on;
xlabel('Czas [s]');
ylabel('Amplituda');
title(sprintf('Przebieg w czasie (kanał %d, %s) — %s', ...
    chan, ternary(C==1,'mono','stereo'), fn), 'Interpreter','none');

%Widmo
nexttile;
plot(fax, P1); grid on;
xlim([0, 2000]);
xlabel('Częstotliwość [Hz]');
ylabel('|X(f)|');
title('Jednostronne widmo amplitudowe (FFT)');
