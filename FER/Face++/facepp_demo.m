%
% Face++ Matlab SDK demo
%
clc; clear;
% Load an image, input your API_KEY & API_SECRET
ims = dir('images/*.jpg');
A = zeros(length(ims), 16);
disp(A);
for k = 1:length(ims),
    img = ['images/' ims(k).name];
    %fprintf('%d', k);
%img = '14.jpg';
API_KEY = 'ENTER API KEY HERE';
API_SECRET = 'ENTER API SECRET HERE';

% If you have chosen Amazon as your API sever and 
% changed API_KEY&API_SECRET into yours, 
% pls reform the FACEPP call as following :
 api = facepp(API_KEY, API_SECRET, 'US')
%api = facepp(API_KEY, API_SECRET);

% Detect faces in the image, obtain related information (faces, img_id, img_height, 
% img_width, session_id, url, attributes)
rst = detect_file(api, img, 'all');
img_width = rst{1}.img_width;
img_height = rst{1}.img_height;
face = rst{1}.face;
fprintf('Frame : %i / %i\n', k, length(ims));
fprintf('Totally %d faces detected!\n', length(face));

im = imread(img);
imshow(im);
hold on;

for i = 1 : length(face)
    % Draw face rectangle on the image
    face_i = face{i};
    center = face_i.position.center;
    w = face_i.position.width / 100 * img_width;
    h = face_i.position.height / 100 * img_height;
    rectangle('Position', ...
        [center.x * img_width / 100 -  w/2, center.y * img_height / 100 - h/2, w, h], ...
        'Curvature', 0.4, 'LineWidth',2, 'EdgeColor', 'blue');
%    fprintf('Centre: %f, %f\n, Height of image: %f, Width of image: %f\n Height : %f, Width : %f', center.x, center.y, img_height, img_width, h, w);
%    cropimg = imcrop(im, [center.x * img_width / 100 -  w/2, center.y * img_height / 100 - h/2, w, h]);
%    figure, imshow(cropimg);
%    fr = getframe(gca);
%    crp = frame2im(fr);
%    imwrite(crp, 'crop.jpg');
    
    % Detect facial key points
    rst2 = api.landmark(face_i.face_id, '83p');
    landmark_points = rst2{1}.result{1}.landmark;
    landmark_names = fieldnames(landmark_points);
    
    % Draw facial key points
    for j = 1 : length(landmark_names)
        pt = getfield(landmark_points, landmark_names{j});
        scatter(pt.x * img_width / 100, pt.y * img_height / 100, 'g.');
    end
    filename = strcat('Data', num2str(k));
    name = strcat(filename, '.txt');
    fileid = fopen(name, 'w');
    for a = 1 : length(landmark_names)
        point = getfield(landmark_points, landmark_names{a});
        fprintf(fileid, '%i. %s : %f, %f\n', a, landmark_names{a}, point.x * img_width / 200, point.y * img_height / 200);
    end
    
    if (k == 1)
        %Calculating step for IRIS Diameter
        left_eye_bottom = getfield(landmark_points, landmark_names{20});
        left_eye_top = getfield(landmark_points, landmark_names{27});
        right_eye_bottom = getfield(landmark_points, landmark_names{66});
        right_eye_top = getfield(landmark_points, landmark_names{73});
        IRISD = ((left_eye_bottom.y * img_height / 200) - (left_eye_top.y * img_height / 200))/1024;
        %IRISDR = ((right_eye_bottom.y * img_height / 200) - (right_eye_top.y * img_height / 200))/1024;
        %fprintf('IRISD is : %f\n', IRISD);
        
        %Calculating step for EyeSeperation
        right_eye_center = getfield(landmark_points, landmark_names{67});
        left_eye_center = getfield(landmark_points, landmark_names{21});
        ES = ((right_eye_center.x * img_width / 200) - (left_eye_center.x * img_width / 200))/1024;
        %fprintf('ES is : %f\n', ES);
        
        %Calculating step for Eyes-Nose Seperation
        nose_contour_lower_middle = getfield(landmark_points, landmark_names{59});
        ENS = ((nose_contour_lower_middle.y * img_height / 200) - (left_eye_center.y * img_height / 200))/1024;
        %fprintf('ENS is : %f\n', ENS);
        
        %Calculating step for Mouth-Nose Seperation
        mouth_upper_lip_bottom = getfield(landmark_points, landmark_names{48});
        MNS = ((mouth_upper_lip_bottom.y * img_height / 200) - (nose_contour_lower_middle.y * img_height / 200))/1024;
        %fprintf('MNS is : %f\n', MNS);
        
        %Calculating step for Mouth Width
        mouth_right_corner = getfield(landmark_points, landmark_names{47});
        mouth_left_corner = getfield(landmark_points, landmark_names{38});
        MW = ((mouth_right_corner.x * img_width / 200) - (mouth_left_corner.x * img_width / 200))/1024;
        %fprintf('MW is : %f\n', MW);
        
        %Calculating step for open_jaw
        contour_chin = getfield(landmark_points, landmark_names{1});
        nose_contour_lower_middle = getfield(landmark_points, landmark_names{59});
        OJ = ((contour_chin.y * img_height / 200) - (nose_contour_lower_middle.y * img_height / 200))/1024
    end
    
    %Calculating open_jaw AU
    contour_chin = getfield(landmark_points, landmark_names{1});
    nose_contour_lower_middle = getfield(landmark_points, landmark_names{59});
    A(k,1) = floor((contour_chin.y * img_height / 200) - (nose_contour_lower_middle.y * img_height / 200))/OJ;
    %disp(A);
    
    %Calculating lower_t_midlip
    mouth_lower_lip_top = getfield(landmark_points, landmark_names{46});
    mouth_upper_lip_bottom = getfield(landmark_points, landmark_names{48});
    A(k,2) = floor((mouth_lower_lip_top.y * img_height / 200) - (mouth_upper_lip_bottom.y * img_height / 200))/MNS;
%    fprintf('%f\n', round((mouth_lower_lip_top.y * img_height / 200) - (mouth_upper_lip_bottom.y * img_height / 200))/MNS);
    %disp(A);
    
    %Calculating stretch_l_cornerlip
    mouth_left_corner = getfield(landmark_points, landmark_names{38});
    A(k,3) = ((mouth_lower_lip_top.x * img_width / 200) - (mouth_left_corner.x * img_width / 200))/MW;
    %disp(A);
    
    %Calculating stretch_r_cornerlip
    mouth_right_corner = getfield(landmark_points, landmark_names{47});
    A(k,4) = ((mouth_right_corner.x * img_width / 200) - (mouth_lower_lip_top.x * img_width / 200))/MW;
    %disp(A);    
    
    %Calculating close_t_l_eyelid
    left_eye_center = getfield(landmark_points, landmark_names{21});
    left_eye_top = getfield(landmark_points, landmark_names{27});
    A(k,5) = ((left_eye_center.y * img_height / 200) - (left_eye_top.y * img_height / 200))/IRISD;
    %disp(A);
    
    %Calculating close_t_r_eyelid
    right_eye_center = getfield(landmark_points, landmark_names{67});
    right_eye_top = getfield(landmark_points, landmark_names{73});
    A(k,6) = ((right_eye_center.y * img_height / 200) - (right_eye_top.y * img_height / 200))/IRISD;
    %disp(A);
    
    %Calculating close_b_l_eyelid
    left_eye_bottom = getfield(landmark_points, landmark_names{20});
    A(k,7) = ((left_eye_bottom.y * img_height / 200) - (left_eye_center.y * img_height / 200))/IRISD;
    %disp(A);
    
    %Calculating close_b_r_eyelid
    right_eye_bottom = getfield(landmark_points, landmark_names{66});    
    A(k,8) = ((right_eye_bottom.y * img_height / 200) - (right_eye_center.y * img_height / 200))/IRISD;
    %disp(A);
    
    %Calculating raise_l_i_eyebrow
    left_eye_right_corner = getfield(landmark_points, landmark_names{26});
    left_eyebrow_right_corner = getfield(landmark_points, landmark_names{34});
    A(k,9) = ((left_eye_right_corner.y * img_height / 200) - (left_eyebrow_right_corner.y * img_height / 200))/ENS;
    %disp(A);
    
    %Calculating raise_r_i_eyebrow
    right_eye_left_corner = getfield(landmark_points, landmark_names{68});
    right_eyebrow_left_corner = getfield(landmark_points, landmark_names{76});
    A(k,10) = ((right_eye_left_corner.y * img_height / 200) - (right_eyebrow_left_corner.y * img_height / 200))/ENS;
    %disp(A);
    
    %Calculating raise_l_m_eyebrow
    left_eyebrow_upper_middle = getfield(landmark_points, landmark_names{36});
    A(k,11) = ((left_eye_top.y * img_height / 200) - (left_eyebrow_upper_middle.y * img_height / 200))/ENS;
    %disp(A);
    
    %Calculating raise_r_m_eyebrow
    right_eyebrow_upper_middle = getfield(landmark_points, landmark_names{82});
    A(k,12) = ((right_eye_top.y * img_height / 200) - (right_eyebrow_upper_middle.y * img_height / 200))/ENS;
    %disp(A);
    
    %Calculating raise_l_o_eyebrow
    left_eye_left_corner = getfield(landmark_points, landmark_names{22});
    left_eyebrow_left_corner = getfield(landmark_points, landmark_names{30});
    A(k,13) = ((left_eye_left_corner.y * img_height / 200) - (left_eyebrow_left_corner.y * img_height / 200))/ENS;
    %disp(A);
    
    %Calculating raise_r_o_eyebrow
    right_eye_right_corner = getfield(landmark_points, landmark_names{72});
    right_eyebrow_right_corner = getfield(landmark_points, landmark_names{80});
    A(k,14) = ((right_eye_right_corner.y * img_height / 200) - (right_eyebrow_right_corner.y * img_height / 200))/ENS;
    %disp(A);
    
    %Calculating squeeze_l_eyebrow
    A(k,15) = ((left_eyebrow_right_corner.x * img_width / 200) - (left_eyebrow_left_corner.x * img_width / 200))/ES;
    %disp(A);
    
    %Calculating squeeze_r_eyebrow
    tri = floor((right_eyebrow_right_corner.x * img_width / 200) - (right_eyebrow_left_corner.x * img_width / 200))/ES;
    A(k,16) = tri;
    disp(A);
    
    if (k == length(ims))
        dlmwrite('AUMatrix.txt', A);
    end
    
end
    fclose(fileid);

    f = getframe(gca);
    im = frame2im(f);
    %fprintf('%d', k);
    imwrite(im, ['modified images/' ims(k).name]);

    %disp('press any key to continue');
    %pause;
    close all;
end
fprintf('Done!\n');


