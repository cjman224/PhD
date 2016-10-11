Hindex = [2	2.5	3.15	4	5	6.3	8	10	12.5	16	20	25	31.5	40	50	63	80	100	125	160	200	250	315]';
MagCBH_acc = [0.014	0.016	0.018	0.018	0.015	0.015	0.019	0.022	0.022	0.025	0.025	0.028	0.03	0.027	0.025	0.025	0.026	0.025	0.032	0.027	0.033	0.044	0.065]';


MagCBH_acc = 1./(MagCBH_acc.*MagCBH_acc);
MagCBH_acc = MagCBH_acc./max(MagCBH_acc);


CriFiltMag = MagCBH_acc;

y = smooth(CriFiltMag,21,'lowess');

x = 0:0.5:5000;
y = interp1([0; Hindex; 5000], [1; y; 0], x);

y(250:end) = smooth(y(250:end),1001,'moving');

Hindex = 0:0.5:22050;
y(end+1:44101) = 0;

plot(Hindex,y);
CriFiltMag = y;

Hindex = Hindex';
CriFiltMag = CriFiltMag';

save('CriBandFilt.mat','Hindex','CriFiltMag');

