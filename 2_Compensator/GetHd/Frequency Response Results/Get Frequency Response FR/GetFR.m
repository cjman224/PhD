Fs = 44100;

load('Measured_Data_0kg.mat');  %% inputVolt, outputAcc

X = fft(inputVolt);
Y = fft(outputAcc);

MagX = abs(X);
MagY = abs(Y);

PhaX = atan2(imag(X),real(X));
for i=1:length(PhaX)
    if(real(X(i))<0 && imag(X(i))<0)
        PhaX(i) = PhaX(i) - pi;
    else if (real(X(i))<0 && imag(X(i))>=0)
            PhaX(i) = PhaX(i) + pi;
        end
    end
end

PhaX = unwrap(PhaX);


PhaY = atan2(imag(Y),real(Y));
for i=1:length(PhaY)
    if(real(Y(i))<0 && imag(Y(i))<0)
        PhaY(i) = PhaY(i) - pi;
    else if (real(Y(i))<0 && imag(Y(i))>=0)
            PhaY(i) = PhaY(i) + pi;
        end
    end
end

PhaY = unwrap(PhaY);


MagH = MagY./MagX;
PhaH = PhaY-PhaX;

MagH2 = MagH(1:length(MagH)/2+1);
PhaH2 = PhaH(1:length(PhaH)/2+1);



[ff, fy] = MYFFTPlot(X,Fs);
[Off, Ofy] = MYFFTPlot(Y,Fs);

figure;
subplot(4,2,1);
plot(time,inputVolt);
title('Input');
xlabel('Time (s)');
ylabel('Voltage (V)');

subplot(4,2,2);
plot(ff(2:end),fy(2:end));
title('Input');
xlabel('Frequency (Hz)');
ylabel('Voltage (V)');
set(gca,'XLim',[0 2500]);

subplot(4,2,3);
plot(time,outputAcc);
title('Output');
xlabel('Time (s)');
ylabel('Acceleration (G)');

subplot(4,2,4);
plot(Off, Ofy);
title('Output');
xlabel('Frequency (Hz)');
ylabel('Acceleration (G)');
set(gca,'XLim',[0 2500]);

subplot(4,2,[5,6]);
plot(ff, medfilt1(MagH2,100));
title('Frequency Response (Mag)');
xlabel('Frequency (Hz)');
ylabel('Magnitude');
set(gca,'XLim',[0 2500]);

subplot(4,2,[7,8]);
plot(ff, medfilt1(PhaH2,100));
title('Frequency Response (Pha)');
xlabel('Frequency (Hz)');
ylabel('Phase');

MagH = medfilt1(MagH2,50);
PhaH = medfilt1(PhaH2,50);
Hindex = ff;
save('FR.mat','MagH','PhaH','Hindex');

%MagH = interp1(ff,MagH(1:length(ff)),iff);

% Reconstruction
revMagH = flipud(MagH);
MagH = [MagH ; revMagH(2:end-1)];
MagY_e = MagX.*MagH;

revPhaH = flipud(PhaH);
PhaH = [PhaH ; revPhaH(2:end-1)];
PhaY_e = PhaX+PhaH;

Yrecon = complex(MagY_e.*cos(PhaY_e),MagY_e.*sin(PhaY_e));
Yrecon(1) =  complex(MagY_e(1).*cos(PhaY_e(1)),0);

% Xrecon = complex(MagX.*cos(PhaX),MagX.*sin(PhaX));
% Xrecon(1) =  complex(MagX(1).*cos(PhaX(1)),0);
% XX = [Xrecon X];
% 
% Yrecon = complex(MagY.*cos(PhaY),MagY.*sin(PhaY));
% Yrecon(1) =  complex(MagY(1).*cos(PhaY(1)),0);
% YY = [Yrecon Y];
% 
% Hrecon = complex(MagH.*cos(PhaH),MagH.*sin(PhaH));
% Hrecon(1) =  complex(MagH(1).*cos(PhaH(1)),0);

figure;
[eff, efy] = MYFFTPlot(Yrecon,Fs);
plot(eff,efy);
hold on;
plot(Off,Ofy);


