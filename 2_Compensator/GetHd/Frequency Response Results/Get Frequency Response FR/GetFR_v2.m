Fs = 44100;

load('Measured_Data_0kg.mat');  %% inputVolt, outputAcc

X = fft(inputVolt);
Y = fft(outputAcc);

MagX = abs(X);
MagY = abs(Y);

PhaX = unwrap(angle(X));
PhaY = unwrap(angle(Y));


MagH = MagY./MagX;
PhaH = PhaY-PhaX;

MagH = MagH(1:length(MagH)/2+1);
PhaH = PhaH(1:length(PhaH)/2+1);

[ff, fy] = MYFFTPlot(X,Fs);
Hindex = ff;


MagH = smooth(MagH, 501, 'lowess');
%MagH(1000:end) = smooth(MagH(1000:end), 1301, 'lowess');
PhaH = smooth(PhaH, 2501, 'lowess');



figure;
subplot(2,1,1);
plot(Hindex,MagH);
set(gca,'xlim',[0 5000]);
ylabel('Magnitude');

% hold on;
% load 'FR_med50.mat';
% plot(Hindex,MagH);

subplot(2,1,2);
plot(Hindex,PhaH);
set(gca,'xlim',[0 5000]);
ylabel('Phase (rad)');

% hold on;
% load 'FR_med50.mat';
% plot(Hindex,PhaH);


save('FR.mat','MagH','PhaH','Hindex');

