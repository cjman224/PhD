Fs = 44100;

x = 1:1:88100;
%y = int16(x);
y = x./100000;


audiowrite('test.wav',y,Fs);