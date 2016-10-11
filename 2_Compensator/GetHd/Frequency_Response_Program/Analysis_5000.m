
% This program is to get data from a text file that contains the results 
% the system identification experiment for the PHANToM frequency response
% July 27, Seungmoon Choi

fid = fopen('Measured_5000Hz_5s_noperson.dat', 'r');
f = fscanf(fid, '%d ', 1); % sampling frequency
sd = fscanf(fid, '%d ', 1); % signal duration

num_samples = f*sd;


average_magnitudez=0;
average_phasez=0;
input_5k = fscanf(fid, '%f ', num_samples);

for i=1:10
    
    z_5k = fscanf(fid, '%f ', num_samples);
    
    % Make IDDATA structure
    dataz = iddata(z_5k, input_5k, 1/f);
    dataz.InputName = {'Voltage'};
    dataz.InputUnit = {'V'};
    dataz.OutputName = {'Acceleration'};
    dataz.OutputUnit = {'g'};
    dataz.TimeUnit = 'sec';
    
    

    spectrum = spa(dataz, 2500, [1:2500]/2500*pi*f); % Play with the second parameter

    
    %spectrum = chgunits(spectrum, 'Hz');
    [Mag,Pha,W,SDMAG,SDPHASE] = ffplot(spectrum);
    

    for j=1:2500
        magnitude(j)=Mag(j);
        phase(j)=Pha(j);
    end

        average_magnitudez = average_magnitudez + magnitude;
        average_phasez = average_phasez + phase;
    

end
fclose(fid);

average_magnitudez = average_magnitudez/10;
average_phasez = average_phasez/10;



figure;
subplot(2,1,1);
plot(W(1:2500), average_magnitudez(1:2500));
xlabel('Hz');
ylabel('Magnitude (G)');
title('PHANToM Frequency Response on Z(Final)');

subplot(2,1,2);
plot(W(1:2500), average_phasez(1:2500));
xlabel('Hz');
ylabel('Phase (Degree)');


fid=fopen('PHANToM_Frequency_Response_Origin_Z.dat', 'w');
fprintf(fid, '%d\n', 2500);
fprintf(fid, '%f ', W);
fprintf(fid, '\n');
fprintf(fid, '%f ', average_magnitudez);
fprintf(fid, '\n');
fprintf(fid, '%f ', average_phasez);
fprintf(fid, '\n');
