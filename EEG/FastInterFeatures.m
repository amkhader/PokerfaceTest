% This Code calculates IntervalFeatures of Erp signal with 256 time frames
% and plots them
% Datapoints are store in A
% 
% __________Output is stored as follows____________________________________
%   mean_n :: N length of intervals
%   std_n  :: N length of intervals
%   coVariance :: Covariance with time

%___________HowTheCodeWorks________________________________________________

%// each 1x4 sliding sub-matrix is made a column
%// for eg:- if A is 1x6 you would get 1-2-3-4, 2-3-4-5, 3-4-5-6 each as a column
%// ending with 3 columns. for 1x10 matrix, you would get 7 columns
   

%// calculating the mean of every column

%// Subtract each value of the column with the mean value of corresponding column
%// finally element-wise squaring, mean of each column 
%// and then element-wise sqrt to get the output.

% A=firstElec;
A= c;



%___________Length2_____________________
reshaped_2 = im2col(A,[1 2],'sliding');
mean_2 = mean(reshaped_2);
out1 = bsxfun(@minus,reshaped_2,mean_2);
std_2 = sqrt(mean(out1.^2));

 %___________Length4_____________________
reshaped_4 = im2col(A,[1 4],'sliding');
mean_4 = mean(reshaped_4);
out2 = bsxfun(@minus,reshaped_4,mean_4);
std_4 = sqrt(mean(out2.^2));

%___________Length8_____________________
reshaped_8 = im2col(A,[1 8],'sliding');
mean_8 = mean(reshaped_8);
out3 = bsxfun(@minus,reshaped_8,mean_8);
std_8 = sqrt(mean(out3.^2));

%___________CoVariance__________________
begInt = 1;
endInt =256;
l=endInt - begInt + 1;
arrayCovar = zeros(62,257);
j=1;

B = parsedChannels;
for j=1:62
for k=1:256
    
    coVar = 1/l *((B(j,k)*k) - (mean(B(j,1:k))*((begInt + endInt)/2)));
    %j= 1+k;
    arrayCovar(j,k)=coVar;
end
end

coVariance = arrayCovar(:,1:256);
disp(coVariance)


% %__________Plotting____________________
% 
% 
% %  y=[mean_2;mean_2];      %//Length 2 Intervals
% %  x=[1:255; 2:256];
% %  plot(x,y)
% %  hold on
% %  plot( A)
% %  
% %  y=[mean_8;mean_8];       %// Length 8 Intervals
% %  x=[1:249; 8:256];
% %  plot(x,y)
% %  hold on
% %  plot( A)
%  
%  y=[mean_4;mean_4];          %//Length 4 Intervals
%  x=[1:253; 4:256];
%  plot(x,y)
%  hold on;
%  plot( A)
%  
%  hold off;

dlmwrite('data.txt', mean_2);
dlmwrite('data.txt', ' ', '-append');
dlmwrite('data.txt', mean_4, '-append');
dlmwrite('data.txt', ' ', '-append');
dlmwrite('data.txt', mean_8, '-append');
dlmwrite('data.txt', ' ', '-append');
dlmwrite('data.txt', std_2, '-append');
dlmwrite('data.txt', ' ', '-append');
dlmwrite('data.txt', std_4, '-append');
dlmwrite('data.txt', ' ', '-append');
dlmwrite('data.txt', std_8, '-append');
dlmwrite('data.txt', ' ', '-append');
dlmwrite('data.txt', coVariance, '-append');

