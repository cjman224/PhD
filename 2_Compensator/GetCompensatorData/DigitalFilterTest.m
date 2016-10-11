Fs = 44100;

%% Loading a Signal Example
xt=audioread('D:\Works\Research\Movies\Gunshots\WavFiles\DirectMapping\10Mov_Gunshots.wav', [ceil(33.08*Fs), ceil(33.5*Fs)]);
if mod(length(xt),2) ~= 0 ; xt = xt(1:end-1); end


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

% b = [0.1 0.1 0.1 0.1 0.1]
% a = 1;
% 
% y = filter(b,a,xt);

% plot(xt, 'Color', 'blue');
% hold on;
% plot(y, 'Color', 'red');


z = [1300; 1300; 1300];
p = [20; 30000; 30000; 30000];
k = 1;
[num, den] = zp2tf(z,p,k);

sys = tf(num,den);

sys_d = c2d(sys,1/Fs,'zoh');

bodeplot(sys_d);

tt = 1/Fs:1/Fs:length(xt)/Fs;
y = lsim(sys_d, xt, tt);
% 

step(sys_d,2.5);







% z=1300*[1; 1; 1]; %% wcut2 = 1300 rad/s.  1 rad/s = 1/2pi Hz, 1 Hz = 2pi rad/s;  1300 rad/s = 206.9 Hz
% p=[20; 30000; 30000; 30000]; %% wcut1 = 20 rad/s;   3.18 Hz.   %% wcut3 = 30,000 rad/s; 4775 Hz
% k = db2mag(-35)*z(1)^3/(p(1)*p(2)*p(3)*p(4));
% k=1/k; %% constant term
% [num, den]=zp2tf(z, p, k);
% sys=tf(num, den);  %% Compensator
% 
% [numd, dend] = bilinear(num,den,Fs);

%bodeplot(sys);


% 
% 
% C_Findex = H_Findex;
% w = 2*pi*C_Findex; %% rad/s unit
% [magC phaC] = bode(sys,w); %% magintude and degree, w should be a rad/TimeUnit(s)
% magC = magC(1,:)';
% phaC = phaC(1,:)';
% 
% save('compensatorData_v3.mat','C_Findex', 'magC', 'phaC'); %% Hz unit, magnitude unit,