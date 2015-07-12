%This segment of the project deals with extracting Interval based features from .......%

1. Import all the .mat files (firstElec, c, parsedChannels) into Matlab.
2. FastIntervalFeatures.m - calculating interval features from parsedChannels.mat
   It calculates length 2, 4, and 8 interval features[mean,standard deviation] and coVariance 
   stored in data.txt file for 62 electrodes operating on 256 Hz sampling frequency.
4. IntervalFeatures.m - calculating interval features from firstElec.mat and plotting them. 
   This is only for one electrode at a time.
5. PowerSpectral.m - Using EEGLAB calculating area under the graph and mean of peak values of eeg signal 
                     separated into four freq bands ALPHA, BETA, THETA, DELTA.
					 
		 ******* IS BEING UPDATED WITH THE LATEST CODE***