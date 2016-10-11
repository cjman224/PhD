
% This program is to get data from a text file that contains the results 
% the system identification experiment for the PHANToM frequency response
% July 27, Seungmoon Choi
% Modulated for Frequency Response on Vibration 
% April, 22, Gunhyuk Park

fid = fopen('Measured_5000Hz_1s.dat', 'r');
f = fscanf(fid, '%d ', 1); % sampling frequency
sd = fscanf(fid, '%d ', 1); % signal duration

num_samples = f*sd;


    x_5k = fscanf(fid, '%f ', num_samples);
    x_5k = transpose(x_5k);
    y_5k = fscanf(fid, '%f ', num_samples);
    y_5k = transpose(y_5k);
    z_5k = fscanf(fid, '%f ', num_samples);
    z_5k = transpose(z_5k);

    % Make a Plot on a Time Domain
    t = 1/f:1/f:sd;

    figure;
    plot(t, x_5k);
    hold on;
%    plot(t, y_5k);
%    plot(t, z_5k);
    
    %% Calculating the FFT of signals
    x_5k_mediate=fftshift(abs(fft(x_5k)));
    y_5k_mediate=fftshift(abs(fft(y_5k)));
    z_5k_mediate=fftshift(abs(fft(z_5k)));
        
    x_acc=(2/sd*x_5k_mediate(length(x_5k_mediate)/2+1:length(x_5k_mediate))/f);       
    y_acc=(2/sd*x_5k_mediate(length(y_5k_mediate)/2+1:length(y_5k_mediate))/f);       
    z_acc=(2/sd*x_5k_mediate(length(z_5k_mediate)/2+1:length(z_5k_mediate))/f);       
    
    ff=0:f/(f*sd):f/2-f/(f*sd);
	

	% Compute displacement
	% 1 G= 9.80665 m/s^2
%	displacement_amplitude = Amp_acc/(2*pi*FF)^2 * 9.80665 * 1000; % now in mm

    hold off
    figure
    plot(ff, x_acc);
    hold on
 %   plot(ff, y_acc);
 %   plot(ff, z_acc);

