%% // Import only the 4th column of data
fid = fopen('testaa.csv') ;
A = textscan( fid , '%*s%*s%*s%f' , 'Delimiter',' ' , 'HeaderLines',4) ;
fclose(fid) ;
A = cell2mat(A) ;

%% // reshape to have each channel in its own colum
A = reshape( A , 257 , [] ) ;

%% // Delete the channel number from the data table
A(1,:) = [] ;

%% // Convert the channel matrix to one vector
A = A(:) ;
A = A';
Z = reshape(A , 256 , [] ) ;
Z = Z';
%% // Intializing
x =0;
arrayCovar = zeros(64,255);

%% // Length 2 Interval Calculation
reshaped_2 = im2col(A,[1 2],'sliding');
mean_2 = mean(reshaped_2);
out1 = bsxfun(@minus,reshaped_2,mean_2);
std_2 = sqrt(mean(out1.^2));
% for row = 1:64
%     for counter = 1:256
%         coVar = 1/2 * (Z(row,counter)*counter) - (mean(Z(row,1:counter)*(3+2^x)/2));
%         x = x + 1;
%         arrayCovar(row,counter)=coVar;
%     end
% end
% coVar = 1/l *((Z(j,k)*k) - (mean(B(j,1:k))*((begInt + endInt)/2)));


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

%% // Covariance Calculation
% begInt = 1;
% endInt =256;
% l=endInt - begInt + 1;
% arrayCovar = zeros(64,257);
% j=1;
% 
% C = reshape(B, 255, []);
% data = C'
% D = data;
% for j=1:64
% for k=1:256
%     
%     coVar = 1/l *((B(j,k)*k) - (mean(B(j,1:k))*((begInt + endInt)/2)));
%     %j= 1+k;
%     arrayCovar(j,k)=coVar;
% end
% end
% coVariance = arrayCovar(:,1:256);
% disp(coVariance);

%% // Storing Mean 2 Length
nMean_2 = mean_2;
mean2Electro = zeros(64,255);

for z=1:63
       nMean_2(z*256)=[];
end
for z=1:64
     mean2Electro(z,:)= nMean_2((z-1)*255 + 1: z*255);
end
mean2Electro = mean2Electro';
A = mean2Electro(:);

%% // Storing Mean 4 Length
nMean_4 = mean_4;
mean4Electro = zeros(64,253);

for z=1:63
       nMean_4(z*254)=[];
end
for z=1:64
     mean4Electro(z,:)= nMean_4((z-1)*253 + 1: z*253);
end

mean4Electro = mean4Electro';
B = mean4Electro(:);

%% // Storing Mean 8 Length
nMean_8 = mean_8;
mean8Electro = zeros(64,249);

for z=1:63
       nMean_8(z*250)=[];
end
for z=1:64
     mean8Electro(z,:)= nMean_8((z-1)*249 + 1: z*249);
end
mean8Electro = mean8Electro';   
C = mean8Electro(:);

%% // Storing Standard Deviation_2
nstd_2 = std_2;
std2Electro = zeros(64,255);

for z=1:63
       nstd_2(z*256)=[];
end
for z=1:64
     std2Electro(z,:)= nstd_2((z-1)*255 + 1: z*255);
end
std2Electro = std2Electro';
D = std2Electro(:);

%% // Storing Standard Deviation_4
nstd_4 = std_4;
std4Electro = zeros(64,253);

for z=1:63
       nstd_4(z*254)=[];
end
for z=1:64
     std4Electro(z,:)= nstd_4((z-1)*253 + 1: z*253);
end

std4Electro = std4Electro';
E = std4Electro(:);

%% // Storing Standard Deviation_8
nstd_8 = std_8;
std8Electro = zeros(64,249);

for z=1:63
       nstd_8(z*250)=[];
end
for z=1:64
     std8Electro(z,:)= nstd_8((z-1)*249 + 1: z*249);
end
std8Electro = std8Electro';
F = std8Electro(:);

ControlSubjectf = cat(1,A,B,C,D,E,F); 

%% // Plotting
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