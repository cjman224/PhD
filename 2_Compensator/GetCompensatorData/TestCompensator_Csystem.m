Fs = 44100;

% %% Loading a Signal Example
% xt=audioread('D:\Works\Research\Movies\Gunshots\WavFiles\DirectMapping\10Mov_Gunshots.wav', [ceil(33.08*Fs), ceil(33.5*Fs)]);
% if mod(length(xt),2) ~= 0 ; xt = xt(1:end-1); end
% 
% %% Applying Compensator
% load('CsystemData_v5.mat'); %% Get 'sysC', compensator system
% 
% sysD = c2d(sysC,1/Fs);
% 
% figure;
% t=0:1/Fs:(length(xt)-1)/Fs;
% y = lsim(sysC, xt ,t);
% subplot(2,1,1);
% plot(xt); bn
% hold on;
% subplot(2,1,2);
% plot(y/1000);


len = 7*60;

y = zeros(len*Fs,1);

%% Applying Compensator
load('CsystemData_v5.mat'); %% Get 'sysC', compensator system
sysD = c2d(sysC,1/Fs);
t=0:1/Fs:1-1/Fs';

for i=1:len
    %% Loading a Signal Example
    xt=audioread('collisions_1ch.wav', [1+(i-1)*Fs, i*Fs]);
    yt = lsim(sysD, xt ,t);
    yt = yt/5;
    y(1+(i-1)*Fs : i*Fs) = yt;
    
end

y = MYCutoffLimit(0.99,y);

audiowrite('collisions_render.wav',y,Fs);

