clc
clear
close all

%plik
[fn, fp] = uigetfile({'*.csv;*.txt','CSV/TXT (*.csv, *.txt)'});
if isequal(fn,0), error('Przerwano wybór pliku.'); end
file = fullfile(fp, fn);

%wczytanie danych
M = readmatrix(file);
if isempty(M) || size(M,2) < 2
    error('Plik "%s" nie zawiera co najmniej 2 kolumn z danymi.', filename);
end

t = M(:,1);  %czas 
x = M(:,2);  %wartości 

%Usunięcie NaN
valid = ~isnan(t) & ~isnan(x);
t = t(valid);
x = x(valid);

%krok probkowania
dt = diff(t);
dt_med = median(dt);
Fs = 1/dt_med; %f próbkowania

%wykresy
figure; 
plot(t, x, 'LineWidth', 1);
grid on;
xlabel('Czas [s]');
ylabel('Wartość ciśnienia [Pa]');
title('Przebieg w czasie');
% xline(0.0024, 'r');
% xline(0.0045, 'r');
% xline(0.0128, 'r');
% xline(0.0157, 'r');

%fft
x2 = x;
x = x(2470:end);
N = numel(x);
xw = x;

X = fft(xw);
P2 = abs(X / N);                    
% części jednostronnej:
if mod(N,2)==0
    % N parzyste
    P1 = P2(1:N/2);
else
    % N nieparzyste
    P1 = P2(1:(N+1)/2);
end
P1(2:end-1) = 2*P1(2:end-1);        

%częstotliwości
f = Fs * (0:numel(P1)-1) / N;

%Wykres widma
figure;
plot(f, P1, 'LineWidth', 1);
grid on;
xlim([0, Fs/2]);
xlabel('Częstotliwość [Hz]');
ylabel('|Y(f)|');
title('Jednostronne widmo amplitudowe (FFT)');
ax = gca;

figure;
subplot(2,1,1);
plot(t, x2, 'LineWidth', 1);
grid on;
xlabel('Czas [s]');
ylabel('Wartość ciśnienia [Pa]');
title('Przebieg w czasie');
subplot(2,1,2);
plot(f, P1, 'LineWidth', 1);
grid on;
xlim([0, Fs/2]);
xlabel('Częstotliwość [Hz]');
ylabel('|Y(f)|');
title('Jednostronne widmo amplitudowe (FFT)');
