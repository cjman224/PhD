Fs = 44100;

load Measured_Gunshot1_0kg;
load FR_med50;

X = fft(inputVolt);
Y = fft(outputAcc);

[iff,ify] = MYFFTPlot(X,Fs);
[Off, Ofy] = MYFFTPlot(Y,Fs);

MagX = abs(X);

% PhaX = atan(imag(X)./real(X));
% for i=1:length(PhaX)
%     if(real(X(i))<0 && imag(X(i))<0)
%         PhaX(i) = PhaX(i) - pi;
%     else if (real(X(i))<0 && imag(X(i))>=0)
%             PhaX(i) = PhaX(i) + pi;
%         end
%     end
% end
PhaX = atan2(imag(X),real(X));

% PhaX = unwrap(PhaX);
PhaX = unwrap(PhaX);


MagH_intp = interp1(Hindex,MagH,iff);
revMagH_intp = flipud(MagH_intp);
MagH_intp = [MagH_intp ; revMagH_intp(2:end-1)];


PhaH_intp = interp1(Hindex,PhaH,iff);
revPhaH_intp = flipud(PhaH_intp);
PhaH_intp = [PhaH_intp ; revPhaH_intp(2:end-1)];

MagY_est = MagX.*MagH_intp;
PhaY_est = PhaX+PhaH_intp;

FY_est = complex(MagY_est.*cos(PhaY_est),MagY_est.*sin(PhaY_est));
FY_est(1) = complex(MagY_est(1).*cos(PhaY_est(1)),0);

[eff,efy] = MYFFTPlot(FY_est,Fs);
ideal_outputT = ifft(FY_est);


figure;
subplot(4,2,1);
plot(time,inputVolt);
title('Input');
xlabel('Time (s)');
ylabel('Voltage (V)');

subplot(4,2,2);
plot(iff(2:end),ify(2:end));
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
title('Comparison');
plotinputT = plot(time,inputVolt);
set(plotinputT,'DisplayName','Input');
hold on;
plotoutputT = plot(time,outputAcc);
set(plotoutputT,'DisplayName','Measured Output');
hold on;
plotidealoutputT = plot(time,ideal_outputT);
set(plotidealoutputT,'DisplayName','Input x Frequency Response');
legend(gca,'show');
xlabel('Time (s)');
ylabel('Acceleration (G)');
set(gca, 'YLim', [-1 ; 1]);


subplot(4,2,[7,8]);
title('Comparison');
plotinputF = plot(iff,medfilt1(ify,50));
set(plotinputF,'DisplayName','Input (Voltage Unit)');
hold on;
plotoutputF = plot(Off, medfilt1(Ofy,50));
set(plotoutputF,'DisplayName','Measured Output');
hold on;
plotoutput_estF = plot(eff, medfilt1(efy,50));
set(plotoutput_estF,'DisplayName','Input x Frequency Response');
legend(gca,'show');
set(gca, 'XLim', [0 ; 2500]);
xlabel('Frequency (Hz)');
ylabel('Acceleration (G)');

    