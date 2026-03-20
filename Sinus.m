clear; clc;

%parametry
fs       = 100000;            %f próbkowania
f        = 150;              %f sinusa
duration = 50*(1/f);         %czas
A        = 0.8;              %amplituda 
phi      = 0;                %poczatkowa faza
basename = 'sine_100Hz';     %plik wynikowy

%generuj
t = (0:1/fs:duration).';          
y = A * sin(2*pi*f*t + phi);       %sygnał

%usrednianie w oknach
win_ms        = 1;                                  %długość okna
win_samples   = max(1, round((win_ms/1000)*fs));    %próbki/okno
N             = size(y,1);
C             = size(y,2);                          
num_blocks    = ceil(N / win_samples);

y_avg = zeros(num_blocks, C);
starts = 1:win_samples:N;
ends   = min(starts + win_samples - 1, N);
for k = 1:numel(starts)
    y_avg(k, :) = mean(y(starts(k):ends(k), :), 1);
end

%srodkowy czas okna
t_avg = ((starts(:) + ends(:) - win_samples -1)/2) / fs;

%fs uśrednione
fs_avg = fs / win_samples;   

%zapis do csv
T_orig = table(t, y(:,1), 'VariableNames', {'time_s','sample'});
writetable(T_orig, sprintf('%s_orig.csv', basename));


fprintf('Zapisano CSV: %s_orig.csv  (N=%d)\n', basename, height(T_orig));

%fft
ch = 1;
y0 = y(:, ch);
y1 = y_avg(:, ch);

%oryginał
L0 = length(y0);
Y0 = fft(y0);
P20 = abs(Y0/L0);
P10 = P20(1:floor(L0/2)+1);
P10(2:end-1) = 2*P10(2:end-1);
f0 = fs*(0:floor(L0/2))/L0;

%Po uśrednianiu
L1 = length(y1);
Y1 = fft(y1);
P21 = abs(Y1/L1);
P11 = P21(1:floor(L1/2)+1);
P11(2:end-1) = 2*P11(2:end-1);
f1 = fs_avg*(0:floor(L1/2))/L1;

%wykresy
figure('Name','Czas','Color','w');
tiledlayout(2,1,'TileSpacing','compact');
nexttile;
plot(t, y(:,1)); grid on; xlabel('Czas [s]'); ylabel('Amplituda');
title('Oryginał');
nexttile;
plot(t_avg, y_avg(:,1)); grid on; xlabel('Czas [s]'); ylabel('Amplituda');
title(sprintf('Uśredniony (okno = %d ms)', win_ms));

figure('Name','Widmo','Color','w');
tiledlayout(2,1,'TileSpacing','compact');
nexttile;
plot(f0, P10); grid on; xlim([0, 2*f]); xlabel('Częstotliwość [Hz]'); ylabel('|Y(f)|');
title(sprintf('FFT oryginału (fs = %g Hz, f = %g Hz)', fs, f));
nexttile;
plot(f1, P11); grid on; xlim([0, 2*f]); xlabel('Częstotliwość [Hz]'); ylabel('|Y(f)|');
title(sprintf('FFT uśrednionego (fs_{avg} ≈ %g Hz, okno = %d ms)', fs_avg, win_ms));
