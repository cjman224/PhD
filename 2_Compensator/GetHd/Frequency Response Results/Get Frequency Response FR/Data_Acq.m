Fs = 44100;

%% Data Acquisition
Dur = 2; %% second

%% Generating Input Signal
y = zeros(Dur*Fs,1);
for i=1:Dur*Fs
    randNum = rand(1);
    if randNum < 0.5
        y(i) = -1;
    elseif randNum >= 0.5
        y(i) = 1;
    else
    end
end

inputVolt = y*2.5;
VoltLimit = 4;
inputVolt = MYCutoffLimit(VoltLimit,inputVolt);  %% Volt (V)

outputAcc = zeros(length(inputVolt),1);

Repeat = 10;
for i=1:Repeat
%% Input Cutoff 4V, Playing and Measuring Vib
SensorGain = 5;
Session = daq.createSession('ni');
addAnalogInputChannel(Session,'Dev2','ai2','Voltage');
addAnalogOutputChannel(Session,'Dev2','ao0','Voltage');
Session.Rate = Fs;
queueOutputData(Session,inputVolt);
[captured_y, time] = Session.startForeground();
captured_y = captured_y*SensorGain; 
outputAcc = outputAcc + captured_y;
end

outputAcc = outputAcc/Repeat;  %% Acceleration (G)

save('Measured_Data.mat','inputVolt','time','outputAcc');
