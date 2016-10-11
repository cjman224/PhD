Fs = 44100;

%% Generating Input Signal
y=audioread('F:\Works\Research\Movies\Gunshots\WavFiles\DirectMapping\10Mov_Gunshots.wav', [ceil(32.7*44100),ceil(34.5*44100)]);
inputVolt = y*2.5;
VoltLimit = 4;
inputVolt = MYCutoffLimit(VoltLimit,inputVolt);  %% Volt (V)

outputAcc = zeros(length(inputVolt),1);


%% Input Cutoff 4V, Playing and Measuring Vib
SensorGain = 5;
Session = daq.createSession('ni');
addAnalogInputChannel(Session,'Dev2','ai2','Voltage');
addAnalogOutputChannel(Session,'Dev2','ao0','Voltage');
Session.Rate = Fs;
queueOutputData(Session,inputVolt);
[outputAcc,time] = Session.startForeground();
outputAcc = outputAcc*SensorGain; 

save('Measured_Gunshot1.mat','inputVolt','time','outputAcc');
