clc
clear

load('Data\FR.mat');
Mag_HD = MagH; %Tactile Device system의 magnitude와 phase
Pha_HD = PhaH;
HD_Findex = Hindex;

load('Data\CriBandFilt.mat');
Mag_HM = CriFiltMag; % Mechanoreceptor System의 magnitude
HM_Findex = Hindex;

H_Findex = HD_Findex; %% HD*HM=H 의 주파수 (Hz) 인덱스
Mag_H = Mag_HD.*Mag_HM;
Pha_H = Pha_HD;

w=2*pi*H_Findex;
A=mag2db(Mag_H);
semilogx(w,A);
grid

% z=0;
% p=[2000 2000 2000 2000 2000];
% k=(2000^5)*(10^-3);

z=[2000 2000 2000 2000 2000];
p=[20000 20000 20000 20000 20000];
k=(2000^5)*(10^-3);


[num, den]=zp2tf(z,p,k);
sys=tf(num, den);

sysd = c2d(sys,0.01);

figure;
bodeplot(sys);
% hold off
% grid