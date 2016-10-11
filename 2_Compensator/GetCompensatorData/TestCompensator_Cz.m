Fs = 44100;

%% Loading a Signal Example
xt=audioread('D:\Works\Research\Movies\Gunshots\WavFiles\DirectMapping\10Mov_Gunshots.wav', [ceil(33.08*Fs), ceil(33.5*Fs)]);
if mod(length(xt),2) ~= 0 ; xt = xt(1:end-1); end

%% Applying Compensator
load('compensatorData_v5.mat'); %% Get 'C_Findex', 'magC', 'phaC', Hz index, magnitude, degree phase

X = fft(xt);
[Y yt]= MYMultipleH(X, magC, phaC, C_Findex, Fs);

t = 1/Fs:1/Fs:length(yt)/Fs;
plot(t,yt);
