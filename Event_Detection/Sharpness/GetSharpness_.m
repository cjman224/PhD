Fs = 48000;
win = Fs/10;


%% Loading a Signal Example
xt=audioread('D:\Works\Research\PhD\1_Voice_Removal\Media\Civil_1ch_3.wav');

shp = zeros(floor(length(xt)/win),1);
 
for i=1:floor(length(xt)/win)
    shp(i) = CalcSharpness(xt(1+win*(i-1):win*i),Fs);
end

t = win/Fs:win/Fs:win/Fs*floor(length(xt)/win);
t = t';

%save('sharp_Civil_1','shp','t');

tt= t(1):1/Fs:(length(xt)-Fs)/Fs;
tt= tt';

shp1 = interp1(t,shp,tt);

y = xt(Fs/10:Fs/10+length(tt)-1);

y = y.*shp1*1;

max(y)
min(y)

audiowrite('Civil_shp_3.wav',y,Fs);