Fs = 10000;

%% Data Acquisition
Dur = 1; %% second
 
%% Generating Input Signal
n=1:1:Fs;
y = 0.5*sin(2*pi*550*n/Fs);


w = 0.5*(1-cos(2*pi*n/(length(y)/2-1)));
w(length(y)/4:length(y)-length(y)/4) = 1;


%plot(w.*y);

y = w.*y;


inputVolt = y';
VoltLimit = 4;
inputVolt = MYCutoffLimit(VoltLimit,inputVolt);  %% Volt (V)

outputAcc = zeros(length(inputVolt),1);                                                                                                                                                                                


%% Input Cutoff 4V, Playing and Measuring Vib
SensorGain = 10;
Session = daq.createSession('ni');
addAnalogInputChannel(Session,'Dev1','ai0','Voltage');
addAnalogOutputChannel(Session,'Dev1','ao0','Voltage');
Session.Rate = Fs;
queueOutputData(Session,inputVolt);
[captured_y, time] = Session.startForeground();
captured_y = captured_y*SensorGain; 
outputAcc = captured_y;

% subplot(2,1,1);
% plot(y);
% subplot(2,1,2);
% plot(outputAcc);

%figure;
% [ff, fy] = MYFFTPlot(fft(outputAcc),Fs);
% %plot(ff,fy);
% fy(150);

%save('Measured_Data.mat','inputVolt','time','outputAcc');
