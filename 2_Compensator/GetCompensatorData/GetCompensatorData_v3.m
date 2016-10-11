%% Output voltage�� maximum�� �Ѱ� ���� �ʾҴ�. �����ļ��� ������Ű�� ���� Ȯ���� �ǹ��ִ� ���̹Ƿ�,
%% 5kHz ��ó�� pole�� 3�� �߰��Ͽ� output voltage�� �� �ٿ����� �;� ���� �������Ͽ���.

Fs = 44100;

%% Data load
load('Data\FR.mat');
Mag_HD = MagH; %%Tactile Device system�� magnitude�� phase
Pha_HD = PhaH;
HD_Findex = Hindex;

load('Data\CriBandFilt.mat');
Mag_HM = CriFiltMag; %% Mechanoreceptor System�� magnitude
HM_Findex = Hindex;

%% Transfer Function of Actuator x Perception Filter
H_Findex = HD_Findex; %% HD*HM=H �� ���ļ� (Hz) �ε���
Mag_HM = interp1(HM_Findex, Mag_HM, H_Findex, 'linear');
Mag_H = Mag_HD.*Mag_HM;
Pha_H = Pha_HD;

z=1300*[1; 1; 1]; %% wcut2 = 1300 rad/s.  1 rad/s = 1/2pi Hz, 1 Hz = 2pi rad/s;  1300 rad/s = 206.9 Hz
p=[20; 30000; 30000; 30000]; %% wcut1 = 20 rad/s;   3.18 Hz.   %% wcut3 = 30,000 rad/s; 4775 Hz
k = db2mag(-35)*z(1)^3/(p(1)*p(2)*p(3)*p(4));
k=1/k; %% constant term
[num, den]=zp2tf(z, p, k);
sys=tf(num, den);  %% Compensator

bodeplot(sys);
Cs = sys;

save('CsystemData_v3.mat','Cs'); %% system class

C_Findex = H_Findex;
w = 2*pi*C_Findex; %% rad/s unit
[magC phaC] = bode(sys,w); %% magintude and degree, w should be a rad/TimeUnit(s)
magC = magC(1,:)';
phaC = phaC(1,:)';

save('compensatorData_v3.mat','C_Findex', 'magC', 'phaC'); %% Hz unit, magnitude unit,