%N= length(EEG.data)
%_____________________EEGLAB______________________________________________%

[ALLEEG EEG CURRENTSET ALLCOM] = eeglab;
EEG = pop_importdata('dataformat','array','nbchan',14,'data','parsedChannels','setname','Test','srate',128,'pnts',0,'xmin',0);
[ALLEEG EEG CURRENTSET] = pop_newset(ALLEEG, EEG, 0,'gui','off'); 
[ALLEEG EEG CURRENTSET] = eeg_store(ALLEEG, EEG, CURRENTSET);
EEG = eeg_checkset( EEG );
pop_eegplot( EEG, 1, 1, 1);
EEG = pop_eegfiltnew(EEG, 4, 45, 212, 0, [], 1);
[ALLEEG EEG CURRENTSET] = pop_newset(ALLEEG, EEG, 1,'gui','off'); 
[ALLEEG EEG CURRENTSET] = eeg_store(ALLEEG, EEG, CURRENTSET);

%_____________________ALPHA BAND__________________________________________%
sampRate = 256; %sampling rate of your data
lowerFreq = 8; %lower bound of your frequency band of interest
higherFreq = 13; %upper bound of your frequency band

N=62;
chanNr = 1; %channel number of your lead
storedAlphaAM = zeros(2,62);
storedBetaAM = zeros(2,62);
storedThetaAM = zeros(2,62);
storedDeltaAM = zeros(2,62);

figure,
i=1;

for chanNr = 1:N,
    [power, freq] = spectopo(EEG.data(chanNr, :), 0, sampRate,'plot','off');
    indLow = find(freq==lowerFreq);
    indHigh = find(freq==higherFreq);
    
%     subplot(N,1,chanNr);
    plot(freq(indLow:indHigh),power(indLow:indHigh));
    axis([freq(indLow) freq(indHigh) get(gca,'YLim')]);
    hold on;
    storedAlphaAM(1,chanNr)=trapz(freq(indLow:indHigh),power(indLow:indHigh));
    meanPower = mean(power(freq >= lowerFreq & freq <= higherFreq))
    storedAlphaAM(2,chanNr) = meanPower;
    
end


trapz(freq(indLow:indHigh),power(indLow:indHigh))
 meanPower = mean(power(freq >= lowerFreq & freq <= higherFreq))





%_____________________BETA BAND_________________________________________%
sampRate = 128; %sampling rate of your data
lowerFreq = 14; %lower bound of your frequency band of interest
higherFreq = 30; %upper bound of your frequency band
chanNr = 1; %channel number of your lead

figure,
for chanNr = 1:N,
    [power, freq] = spectopo(EEG.data(chanNr, :), 0, sampRate,'plot','off');
    indLow = find(freq==lowerFreq);
    indHigh = find(freq==higherFreq);
    
    subplot(N,1,chanNr);
    plot(freq(indLow:indHigh),power(indLow:indHigh));
    axis([freq(indLow) freq(indHigh) get(gca,'YLim')]);
    hold on;
    storedBetaAM(1,chanNr)=trapz(freq(indLow:indHigh),power(indLow:indHigh));
    meanPower = mean(power(freq >= lowerFreq & freq <= higherFreq))
    storedBetaAM(2,chanNr) = meanPower;
end


%_____________________THETA BAND _______________________________________%
sampRate = 128; %sampling rate of your data
lowerFreq = 4; %lower bound of your frequency band of interest
higherFreq = 7; %upper bound of your frequency band
chanNr = 1; %channel number of your lead

figure,
for chanNr = 1:N,
    [power, freq] = spectopo(EEG.data(chanNr, :), 0, sampRate,'plot','off');
    indLow = find(freq==lowerFreq);
    indHigh = find(freq==higherFreq);
    
    subplot(N,1,chanNr);
    plot(freq(indLow:indHigh),power(indLow:indHigh));
    axis([freq(indLow) freq(indHigh) get(gca,'YLim')]);
    hold on;
    storedThetaAM(1,chanNr)=trapz(freq(indLow:indHigh),power(indLow:indHigh));
    meanPower = mean(power(freq >= lowerFreq & freq <= higherFreq))
    storedThetaAM(2,chanNr) = meanPower;
end


%_____________________DELTA BAND________________________________________%
sampRate = 128; %sampling rate of your data
lowerFreq = 1; %lower bound of your frequency band of interest
higherFreq = 3; %upper bound of your frequency band
chanNr = 1; %channel number of your lead

figure,
for chanNr = 1:N,
    [power, freq] = spectopo(EEG.data(chanNr, :), 0, sampRate,'plot','off');
    indLow = find(freq==lowerFreq);
    indHigh = find(freq==higherFreq);
    
    subplot(N,1,chanNr);
    plot(freq(indLow:indHigh),power(indLow:indHigh));
    axis([freq(indLow) freq(indHigh) get(gca,'YLim')]);
    hold on;
    storedDeltaAM(1,chanNr)=trapz(freq(indLow:indHigh),power(indLow:indHigh));
    meanPower = mean(power(freq >= lowerFreq & freq <= higherFreq))
    storedDeltaAM(2,chanNr) = meanPower;
end


% computing log spectrum for different frequencies

% [power, freq] = spectopo(EEG.data(chanNr, :), 0, sampRate);

% average power within the predefined frequency range
% meanPower = mean(power(freq >= lowerFreq & freq <= higherFreq))

dlmwrite('Area.txt', storedAlphaAM);
dlmwrite('Area.txt', ' ', '-append');
dlmwrite('Area.txt', storedBetaAM, '-append');
dlmwrite('Area.txt', ' ', '-append');
dlmwrite('Area.txt', storedThetaAM, '-append');
dlmwrite('Area.txt', ' ', '-append');
dlmwrite('Area.txt',storedDeltaAM, '-append');
dlmwrite('Area.txt', ' ', '-append');

