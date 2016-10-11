%subjN = 8;
%data1 = zeros(5*subjN,12);


load 'acc_measure_8.dat';
freq_idx = 0;
revN = 7;

subj_idx = 8;
acc_measure_1 = acc_measure_8;



for i=1:12
    if acc_measure_1(1+revN*(i-1),2) == 10
        freq_idx = 1;
    elseif acc_measure_1(1+revN*(i-1),2) == 20
        freq_idx = 2;
    elseif acc_measure_1(1+revN*(i-1),2) == 35
        freq_idx = 3;
    elseif acc_measure_1(1+revN*(i-1),2) == 50
        freq_idx = 4;
    elseif acc_measure_1(1+revN*(i-1),2) == 70
        freq_idx = 5;
    elseif acc_measure_1(1+revN*(i-1),2) == 95
        freq_idx = 6;
    elseif acc_measure_1(1+revN*(i-1),2) == 120
        freq_idx = 7;
    elseif acc_measure_1(1+revN*(i-1),2) == 150
        freq_idx = 8;
    elseif acc_measure_1(1+revN*(i-1),2) == 200
        freq_idx = 9;
    elseif acc_measure_1(1+revN*(i-1),2) == 280
        freq_idx = 10;
    elseif acc_measure_1(1+revN*(i-1),2) == 400
        freq_idx = 11;
    elseif acc_measure_1(1+revN*(i-1),2) == 550
        freq_idx = 12;
    end
    
    data1(1+5*(subj_idx-1),freq_idx) = acc_measure_1(1+revN*(i-1),2);
    data1(2+5*(subj_idx-1),freq_idx) = mean( acc_measure_1(5+revN*(i-1):7+revN*(i-1),1) );
    data1(3+5*(subj_idx-1),freq_idx) = mean( [rms(acc_measure_1(5+revN*(i-1),5:end)); rms(acc_measure_1(6+revN*(i-1),5:end)); rms(acc_measure_1(7+revN*(i-1),5:end))] );
    
end


result = zeros(3,12);

for i=1:12
    result(1,i) = data1(1,i);
    result(2,i) = mean(data1([2,7,12,17,22,27,32,37],i));
    result(3,i) = mean(data1([3,8,13,18,23,28,33,38],i));
end



