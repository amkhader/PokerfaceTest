% This Code calculates IntervalFeatures of Erp signal with 256 time frames
% and plots them
% Datapoints are store in A
% 
% __________Output is stored as follows____________________________________
% 
% storedM :: Matrix containing Mean of the data
% storedS :: Matrix containing StandardDeviation of the data
% coVariance :: Array containing Covariance with time
% 




A = parsedChannels(2,:);
sum=0;
storedM = zeros(3,255);
storedS = zeros(3,255);
% storedPC = zeros(186,255);

begInt = 1;
endInt =256;


for k=1:255
    n=1;
    while(1==1)
        if(k+(2^n-1)<=256)
%           sum= A(k) + A(k+2^n-1); 
            meanSum= mean(A(k:k+2^n-1));
                        
            storedM(n,k)= meanSum
            storedM_len2 = storedM(1,:);
            
%             covVar= cov(A(k:k+2^n-1));
%             storedC(n,k)= covVar
            stdDev= std(A(k:k+2^n-1));
            storedS(n,k)= stdDev;
%             standDev= (A(k)-meanSum)^2/
            
            
            n=n+1;
        else
            break;
        end
    end
end

l=endInt - begInt + 1;
arrayCovar = zeros(1,257);
j=1;
for k=1:256
    
    coVar = 1/l *((A(k)*k) - (mean(A(k:j))*((begInt + endInt)/2)));
    j= 1+k;
    arrayCovar(k)=coVar;
end

coVariance = arrayCovar(:,1:256);
disp(coVariance)
    
    
%  y=[storedM(1,:);storedM(1,:)];      %//Length 2 Intervals
%  x=[1:255; 2:256];
%  plot(x,y)
%  hold on
%  plot( A)
%  
%  y=[storedM(3,:);storedM(3,:)];       %// Length 8 Intervals
%  x=[1:255; 8:262];
%  plot(x,y)
%  hold on
%  plot( A)
 
 y=[storedM(2,:);storedM(2,:)];          %//Length 4 Intervals
 x=[1:255; 4:258];
 plot(x,y)
 hold on;
 plot( A)
 
 hold off;
 
