fid = fopen('Frequency_Response_Fs5000Hz_noperson.dat', 'r');
n = fscanf(fid, '%d ', 1); 
y = fscanf(fid, '%f ', [n,inf]);

index = 2;
yseg = y(:,index);
plot(yseg);


