%% v1���� ���� control system�� zero�� pole���� ���Ƽ� causal���� �ʴ�
%% ����, v2������ pole�� �� �� �߰��Ͽ� causal�� sysem�� �����غ��Ҵ�.

Fs = 44100;

%% Data load
load('Data\FR.mat');
Mag_HD = MagH; %%Tactile Device system�� magnitude�� phase
Pha_HD = PhaH;
HD_Findex = Hindex;

load('Data\CriBandFilt.mat');
Mag_HM = [1; CriFiltMag; 0]; %% Mechanoreceptor System�� magnitude�� phase
HM_Findex = [0; Hindex; Fs/2];

%% Transfer Function of Actuator x Perception Filter
H_Findex = HD_Findex; %% HD*HM=H �� ���ļ� (Hz) �ε���
Mag_HM = interp1(HM_Findex, Mag_HM, H_Findex, 'linear');
Mag_H = Mag_HD.*Mag_HM;
Pha_H = Pha_HD;

z=1400*[1; 1; 1]; %% wcut2 = 1400 rad/s.  1 rad/s = 1/2pi Hz, 1 Hz = 2pi rad/s;  1400 rad/s = 222.82 Hz
p=[20; 300000; 300000]; %% wcut1 = 20 rad/s;   3.18 Hz.   %% wcut3 = 300,000 rad/s; 47746 Hz
k = db2mag(-35)*z(1)^3/(p(1)*p(2)*p(3));
k=1/k; %% constant term
[num, den]=zp2tf(z, p, k);
sys=tf(num, den);  %% Compensator

bodeplot(sys);
Cs = sys;

save('CsystemData_v2.mat','Cs'); %% system class