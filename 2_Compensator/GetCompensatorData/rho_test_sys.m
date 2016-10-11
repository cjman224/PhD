clc
clear

% RC circuit analysis, just for test
% It is a low pass filter, when we increase the freq of sin wave
% we almost get nothing in the output.
R=1e5;
C=1e-6;
tau=R*C;
num=1/tau;
den=[1, 1/tau];
sys=tf(num, den);

figure(1);
w = linspace(0,1000,1000);
[mag, ph]=bode(sys, w);
mag = squeeze(mag);
A=mag2db(mag);
semilogx(w, A);
grid;

figure(2);
bodemag(sys);
grid;

%frd1 = frd(A, w);
%sys2 = tfest(frd1, 4);
figure(3);
t=0:0.001:2;
%u=(rem(t,1) >= 0.5);
u=sin(10*t);
lsim(sys, u, t);
grid

figure(4);
y = filter (den, num, u);
plot(t, y);
hold on
plot(t, u);
grid
hold off

% load('Data\FR.mat');
% Mag_HD = MagH; %Tactile Device system의 magnitude와 phase
% Pha_HD = PhaH;
% HD_Findex = Hindex;
% 
% load('Data\CriBandFilt.mat');
% Mag_HM = CriFiltMag; % Mechanoreceptor System의 magnitude
% HM_Findex = Hindex;
% 
% H_Findex = HD_Findex; %% HD*HM=H 의 주파수 (Hz) 인덱스
% Mag_H = Mag_HD.*Mag_HM;
% Pha_H = Pha_HD;
% 
% w=2*pi*H_Findex;
% A=mag2db(Mag_H);
% semilogx(w,A);
% grid
% 
% z=0;
% p=[2000 2000 2000 2000 2000];
% k=(2000^5)*(10^-3);
% [num, den]=zp2tf(z,p,k);
% sys=tf(num, den);
% 
% % hold on
% % bodemag(sys);
% % hold off
% % grid