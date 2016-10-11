Fs = 44100;

%% Generating Input Signal
% y = zeros(Fs,1);
% for i=1:Fs
%     y(i) = sin(2*pi*10*i/Fs)+sin(2*pi*20*i/Fs)+sin(2*pi*50*i/Fs)+sin(2*pi*100*i/Fs)+sin(2*pi*200*i/Fs)+sin(2*pi*300*i/Fs)+sin(2*pi*400*i/Fs)+sin(2*pi*500*i/Fs);
%     y(i) = y(i) + sin(2*pi*600*i/Fs)+sin(2*pi*700*i/Fs)+sin(2*pi*800*i/Fs)+sin(2*pi*900*i/Fs)+sin(2*pi*1000*i/Fs);
% end
% 
% y = y./3.33;

y=audioread('F:\Works\Research\Movies\Gunshots\WavFiles\DirectMapping\10Mov_Gunshots.wav', [ceil(32.7*44100),ceil(34.5*44100)]);
y = y.*2.5;
% figure;
% subplot(2,1,1);
% plot(y);
% xlabel('Sample Number');
% ylabel('Voltage (V)');



%% Playing and Measuring Vib
Session = daq.createSession('ni');
addAnalogInputChannel(Session,'Dev2','ai2','Voltage');
addAnalogOutputChannel(Session,'Dev2','ao0','Voltage');
Session.Rate = Fs;
inputData = y;
VoltLimit = 4;
inputData = MYCutoffLimit(VoltLimit,inputData);
% queueOutputData(Session,inputData);
% 
% [captured_y,time] = Session.startForeground();
% subplot(2,1,2);
% plot(time,captured_y);
% xlabel('Sample Number');
% ylabel('Acceleration (G)');
% 
% save('MeasuredOutput.txt',captured_y);


%% Multiplying Frequency Response to Input Signal
figure;
fid = fopen('Frequency_Response_Fs5000Hz_noperson.dat', 'r');     %% Load Frequency Response
fresp_maxf = fscanf(fid, '%d ', 1); 
fresp = fscanf(fid, '%f ', [fresp_maxf,inf]);

[inputFf, inputFy] = MYFFT(inputData,Fs); 
% plot1 = plot(inputFf, inputFy);
% set(plot1,'DisplayName','Input');
% set(gca, 'XLim', [0 ; 2500]);

fresp_interp = interp1(fresp(:,1),fresp(:,2),inputFf, 'spline', 'extrap'); 
fresp_interp(fresp_maxf:end) = 0;

cnt = 1;
outputFy_est = zeros(length(inputFy),1);
while (inputFf(cnt)<=fresp_maxf)
    outputFy_est(cnt) = inputFy(cnt) * fresp_interp(cnt); % Original signal 의 binFreq*i 번째 주파수 성분  *  freq response의 binFreq*i 번째 주파수 성분
    cnt = cnt + 1;
end

figure;
plot(inputFf, outputFy_est);
set(gca, 'XLim', [0 ; 2500]);


frespData = MYIFFT(fresp_interp,Fs);
outputFy_est2 = conv(inputData,frespData);
[ff_est,fy_est] = MYFFT(outputFy_est2,Fs);
figure; plot(ff_est,fy_est);
set(gca, 'XLim', [0 ; 2500]);


% 
% figure; plot1 = plot(ff,inputFy);
% set(plot1,'DisplayName','Input');
% ax1 = gca;
% set(ax1, 'XLim', [0 ; 1500]);
% xlabel(ax1,'Frequency (Hz)');
% ylabel(ax1,'Voltage (V)');
% 
% hold on;
% plot2 = plot(ff,fy);
% set(plot2,'DisplayName','Input x Freq Response');
% ax2 = gca;
% set(ax2, 'XLim', [0 ; 1500]);
% xlabel(ax2,'Frequency (Hz)');
% ylabel(ax2,'Acceleration (G)');
% 
% hold on;
% output = load('MeasuredOutput.txt');
% plot3 = plot(output(1,:),output(2,:));
% set(plot3,'DisplayName','Measured output');
% legend(ax2,'show');
    
%T_fresp = MYIFFT(fresp_int

    