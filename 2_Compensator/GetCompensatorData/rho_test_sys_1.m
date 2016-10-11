clc
clear

load('Data\FR.mat');
Mag_HD = MagH; %Tactile Device system�� magnitude�� phase
Pha_HD = PhaH;
HD_Findex = Hindex;

load('Data\CriBandFilt.mat');
Mag_HM = CriFiltMag; % Mechanoreceptor System�� magnitude
HM_Findex = Hindex;

H_Findex = HD_Findex; %% HD*HM=H �� ���ļ� (Hz) �ε���
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