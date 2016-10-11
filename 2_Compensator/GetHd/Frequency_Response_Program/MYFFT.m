function [ y, ff ] = MYFFT(x, Fs)

N = 8192;
TotDur = length(x)/Fs;
fx = fft(x);
fx = abs(fx/length(fx));
fx2 = fx(1:length(fx)/2+1);
fx2(2:end-1) = 2*fx2(2:end-1);

ff = Fs*(0:(length(fx)/2))/length(fx);
y = fx2;

end

