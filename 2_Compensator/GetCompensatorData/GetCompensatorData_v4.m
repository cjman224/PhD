%% Digital controller를 구현하는 것이 기본 목적이다.
%% FR.mat, CriBandFilt.mat이 조금 바뀌었다.

Fs = 44100;

%% Data load
load('Data\FR.mat');
Mag_HD = MagH; %%Tactile Device system의 magnitude와 phase
Pha_HD = PhaH;
HD_Findex = Hindex;

load('Data\CriBandFilt.mat');
Mag_HM = CriFiltMag; %% Mechanoreceptor System의 magnitude
HM_Findex = Hindex;

%% Transfer Function of Actuator x Perception Filter
H_Findex = HD_Findex; %% HD*HM=H 의 주파수 (Hz) 인덱스
Mag_H = Mag_HD.*Mag_HM;
Pha_H = Pha_HD;

% subplot(2,1,1);
% plot(H_Findex,Mag_H);
% subplot(2,1,2);
% plot(H_Findex,Pha_H);

z=1300*[1; 1; 1]; %% wcut2 = 1300 rad/s.  1 rad/s = 1/2pi Hz, 1 Hz = 2pi rad/s;  1300 rad/s = 206.9 Hz
p=[20; 30000; 30000; 30000]; %% wcut1 = 20 rad/s;   3.18 Hz.   %% wcut3 = 30,000 rad/s; 4775 Hz
k = db2mag(-35)*z(1)^3/(p(1)*p(2)*p(3)*p(4));
k=1/k; %% constant term
[num, den]=zp2tf(z, p, k);
sys=tf(num, den);  %% Compensator

[numd, dend] = bilinear(num,den,Fs);

%bodeplot(sys);


% 
save('CsystemData_v4.mat','numd','dend'); %% system class
% 
% C_Findex = H_Findex;
% w = 2*pi*C_Findex; %% rad/s unit
% [magC phaC] = bode(sys,w); %% magintude and degree, w should be a rad/TimeUnit(s)
% magC = magC(1,:)';
% phaC = phaC(1,:)';
% 
% save('compensatorData_v3.mat','C_Findex', 'magC', 'phaC'); %% Hz unit, magnitude unit,