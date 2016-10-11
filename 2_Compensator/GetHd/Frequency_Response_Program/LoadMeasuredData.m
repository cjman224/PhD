fid = fopen('Measured_44100Hz_5s.dat', 'r');
Fs = fscanf(fid, '%d ', 1); % sampling frequency
sd = fscanf(fid, '%d ', 1); % signal duration
num_samples = Fs*sd;
x = fscanf(fid, '%f ', num_samples);
y = fscanf(fid,'%f', [num_samples,inf]);

index = 1;
yseg = y(:,index);
plot(yseg);

[fy,ff]=MYFFT(x, Fs);
%plot(ff,fy);
