%_____________WORKING WITH DATA FROM EEGLAB_______________________________%


%% // IMporting Exported epochs from eeglab
newData1 = importdata('C:\Users\harsh\Downloads\eeglab13_4_4b\ferdav1.csv', '\t',1); % cd to the file exported from eeglab
Data= newData1.data;
Data = Data';

%% // IMporting Eye Tracking file
fid = fopen('C:\Users\harsh\Downloads\eeglab13_4_4b\ferda1eye.csv') ; % cd to the file generated by Eye Tracking
A = textscan( fid , '%s %f %u %s ' , 'Delimiter',',' , 'HeaderLines',0) ;
fclose(fid) ;

[d1,d2] = size(Data);
length = (d1/128); % gives number of words eye tracking detected

%% // Splitting data into separate 128x14 blocks
out = reshape(Data,128,14,length);
format = repmat('%f', [1 14]);
Words = zeros(10444,length);

%% // Major Loop for storing data for classification

for i = 1:length
    word = out(:,:,i);
    textfilename = ['word' num2str(i) '.txt'];
    fid1 = fopen(textfilename, 'wt');
    fprintf(fid1,format, word); % prints ERP data of each word to a .txt file stored in pwd
    
    A = word(:); % Vectorizes the data into a column
    A = A'; % transposes it to a row vector
    
    %% // Length 2 Interval Calculation
    reshaped_2 = im2col(A,[1 2],'sliding'); 
    mean_2 = mean(reshaped_2);
    out1 = bsxfun(@minus,reshaped_2,mean_2);
    std_2 = sqrt(mean(out1.^2));
        
    %% // Length 4 Interval Calculation
    reshaped_4 = im2col(A,[1 4],'sliding');
    mean_4 = mean(reshaped_4);
    out2 = bsxfun(@minus,reshaped_4,mean_4);
    std_4 = sqrt(mean(out2.^2));
    
    %% // Length 8 Interval Calculation
    reshaped_8 = im2col(A,[1 8],'sliding');
    mean_8 = mean(reshaped_8);
    out3 = bsxfun(@minus,reshaped_8,mean_8);
    std_8 = sqrt(mean(out3.^2));
    
    %% // Storing Mean 2 Length
    nMean_2 = mean_2;
    mean2Electro = zeros(14,127);
    
    for z=1:13
        nMean_2(z*128)=[];
    end
    for z=1:14
        mean2Electro(z,:)= nMean_2((z-1)*127 + 1:z*127);
    end
    mean2Electro = mean2Electro';
    A = mean2Electro(:);
    
    %% // Storing Mean 4 Length
    nMean_4 = mean_4;
    mean4Electro = zeros(14,125);
    
    for z=1:13
        nMean_4(z*126)=[];
    end
    for z=1:14
        mean4Electro(z,:)= nMean_4((z-1)*125 + 1: z*125);
    end
    
    mean4Electro = mean4Electro';
    B = mean4Electro(:);
    
    %% // Storing Mean 8 Length
    nMean_8 = mean_8;
    mean8Electro = zeros(14,121);
    
    for z=1:13
        nMean_8(z*122)=[];
    end
    for z=1:14
        mean8Electro(z,:)= nMean_8((z-1)*121 + 1: z*121);
    end
    mean8Electro = mean8Electro';
    C = mean8Electro(:);
    
    %% // Storing Standard Deviation_2
    nstd_2 = std_2;
    std2Electro = zeros(14,127);
    
    for z=1:13
        nstd_2(z*128)=[];
    end
    for z=1:14
        std2Electro(z,:)= nstd_2((z-1)*127 + 1: z*127);
    end
    std2Electro = std2Electro';
    D = std2Electro(:);
    
    %% // Storing Standard Deviation_4
    nstd_4 = std_4;
    std4Electro = zeros(14,125);
    
    for z=1:13
        nstd_4(z*126)=[];
    end
    for z=1:14
        std4Electro(z,:)= nstd_4((z-1)*125 + 1: z*125);
    end
    
    std4Electro = std4Electro';
    E = std4Electro(:);
    
    %% // Storing Standard Deviation_8
    nstd_8 = std_8;
    std8Electro = zeros(14,121);
    
    for z=1:13
        nstd_8(z*122)=[];
    end
    for z=1:14
        std8Electro(z,:)= nstd_8((z-1)*121 + 1: z*121);
    end
    std8Electro = std8Electro';
    F = std8Electro(:);

 

    
    Words(:,i) = cat(1,A,B,C,D,E,F); 
    fclose(fid1);
end

 
Words = Words'; %% All of the words as rows and their features in columns

%% // Plotting

%  
%  y=[mean_2;mean_2];      %//Length 2 Intervals
%  x=[1:127; 2:128];
%  plot(x,y)
%  hold on
%  plot(x,y)
% %  
% %  figure,
% %  y=[mean_8;mean_8];       %// Length 8 Intervals
% %  x=[1:121; 8:128];
% %  plot(x,y)
% %  hold on
% %  plot( A)
% %  
% %  figure,
% %  y=[mean_4;mean_4];          %//Length 4 Intervals
% %  x=[1:125; 4:128];
% %  plot(x,y)
% %  hold on;
% %  plot( A)
%  
%  hold off;