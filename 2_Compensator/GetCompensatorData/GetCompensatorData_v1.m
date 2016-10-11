%% analog controller를 디자인하였다.
%% 하지만 zero가 pole보다 많아서 lsim function을 사용할 수 없다. 따라서 v2를 만듬.

Fs = 44100;

%% Data load
load('Data\FR.mat');
Mag_HD = MagH; %%Tactile Device system의 magnitude와 phase
Pha_HD = PhaH;
HD_Findex = Hindex;

load('Data\CriBandFilt.mat');
Mag_HM = [1; CriFiltMag; 0]; %% Mechanoreceptor System의 magnitude와 phase
HM_Findex = [0; Hindex; Fs/2];

%% Transfer Function of Actuator x Perception Filter
H_Findex = HD_Findex; %% HD*HM=H 의 주파수 (Hz) 인덱스
Mag_HM = interp1(HM_Findex, Mag_HM, H_Findex, 'linear');
Mag_H = Mag_HD.*Mag_HM;
Pha_H = Pha_HD;

z=1400*[1; 1; 1]; %% wcut2 = 1400 rad/s.  1 rad/s = 1/2pi Hz, 1 Hz = 2pi rad/s;  1400 rad/s = 222.82 Hz
p=[20]; %% wcut1 = 20 rad/s;   3.18 Hz.
k = db2mag(-35)*z(1)^3/p(1);
k=1/k; %% constant term
[num, den]=zp2tf(z, p, k);
sys=tf(num, den);  %% Compensator

C_Findex = H_Findex;
w = 2*pi*C_Findex; %% rad/s unit
[magC phaC] = bode(sys,w); %% magintude and degree, w should be a rad/TimeUnit(s)
magC = magC(1,:)';
phaC = phaC(1,:)';

save('compensatorData.mat','C_Findex', 'magC', 'phaC'); %% Hz unit, magnitude unit,