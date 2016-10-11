%% Digital Controller를 다시 구현해보았다.

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

z=[0];
p=[-2000 -2000 -2000 -2000 -2000];
k=(2000^5)*(10^-3);
[num, den]=zp2tf(z,p,k);
sys=tf(num, den);

w = 2*pi*H_Findex;
[mag, pha] = bode(sys,w);  %%mag in mag, pha in degree
%bodeplot(sys);

figure;
mag = mag(1,:)';
pha = pha(1,:)';
mag_dB = mag2db(mag);
semilogx(w,mag_dB); %%mag in dB
%semilogx(w,pha);

hold on;
Mag_H_dB = mag2db(Mag_H);
semilogx(w,Mag_H_dB);


%% Get Compensator
zC=[-2000; -2000; -2000; -2000; -2000];
pC= [-1; -20000; -20000; -20000; -20000; -20000];
kC=(2000^5)*(10^-3)/(20000^5);
kC = 1/kC;
[numC, denC]=zp2tf(zC,pC,kC);
sysC=tf(numC, denC);

[magC, phaC] = bode(sysC,w);  %%mag in mag, pha in degree
%bodeplot(sys);

hold on;
magC = magC(1,:)';
phaC = phaC(1,:)';
magC_dB = mag2db(magC);
semilogx(w,magC_dB); %%mag in dB
%semilogx(w,pha);

C_Findex = H_Findex;

save('CsystemData_v5.mat','sysC');
 
save('compensatorData_v5.mat', 'magC','phaC','C_Findex');