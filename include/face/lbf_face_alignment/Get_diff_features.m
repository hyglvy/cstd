function [pixel_diff_features,pixelpair]=Get_diff_features(radius,imgTr,shape_curstage,idsample,i_point,num_samples,num_features)%,tf2)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  
%�������ܣ���ò�ֵ����
%���룺
%radius��Բ��뾶
%imgTr��ѵ����������
%shape_curstage����ǰ������״
%idsample�����ڵ�������
%i_point����ǰ������
%num_samples��������
%num_features������������
%�����
%pixel_diff_features ��ֵ����
%pixelpair����ֵ������Ӧ�ĽǶ��뼫�뾶
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%       
    [pixel_coordinate,pixelpair]=Get_Coordinate(radius,num_features);
    
    for i_sample = 1:length(idsample)%��ÿһ��ѵ������
        i_Tr=idsample(i_sample)-num_samples*(ceil(idsample(i_sample)/num_samples)-1);%���������
        imgTr_gray=imgTr{i_Tr};
        [height,width]=size(imgTr_gray);
        shape=shape_curstage(idsample(i_sample),:);num_points=size(shape_curstage,2)/2;
        bbox3=max(shape(1:num_points))-min(shape(1:num_points));%��ǰ������
        bbox4=max(shape(1+num_points:end))-min(shape(1+num_points:end));%��ǰ������
        x0=shape_curstage(idsample(i_sample),i_point); y0=shape_curstage(idsample(i_sample),i_point+68);%��ǰ������
        %�õ���ǰ������1000�����ص�ʵ�����꼰�Ҷ�ֵ
        pixel_coordinate_x=pixel_coordinate(1:length(pixel_coordinate)/2)*bbox3;%�������x
        pixel_coordinate_y=pixel_coordinate(1+length(pixel_coordinate)/2:end)*bbox4;
     
        pixel_coordinate_x=round(pixel_coordinate_x+x0);%��������x
        pixel_coordinate_y=round(pixel_coordinate_y+y0);
        
        pixel_coordinate_x=max(min(pixel_coordinate_x,width),1);%����Լ��
        pixel_coordinate_y=max(min(pixel_coordinate_y,height),1);
        
        pixel_a_x=pixel_coordinate_x(1:length(pixel_coordinate_x)/2);    %ǰһ�����ص��x����ֵ
        pixel_b_x=pixel_coordinate_x(1+length(pixel_coordinate_x)/2:end);%��һ�����ص��x����ֵ
        pixel_a_y=pixel_coordinate_y(1:length(pixel_coordinate_y)/2);    %ǰһ�����ص��y����ֵ
        pixel_b_y=pixel_coordinate_y(1+length(pixel_coordinate_y)/2:end);%��һ�����ص��y����ֵ
        
        pixel_diff_features(i_sample,:)=int16(imgTr_gray(pixel_a_y + (pixel_a_x-1)*height))-...
                                        int16(imgTr_gray(pixel_b_y + (pixel_b_x-1)*height));%���ز�����
    end
end

function [pixel_coordinate,pixelpair]=Get_Coordinate(radius,num_features)
%�������ܣ�
%��þֲ��뾶����������
%���ü������ʾ����ת��ֱ������
        radius_grid = ([0:1/30:1]');%�Ѱ뾶����
        angles_grid = 2*pi*[0:1/36:1]';%�ѽǶȾ���
        [radiuspairs, anglepairs] = getproposals(num_features,radius_grid,angles_grid);%�õ�����İ뾶�Լ��Ƕȶԣ������꣩
        pixelpair=[anglepairs radiuspairs];%���ض�
        angles_cos = cos(anglepairs);%x����Ϊcos
        angles_sin = sin(anglepairs);%y����Ϊsin
        pixel_coordinate=[(angles_cos(:, 1).*radiuspairs(:, 1)*radius)',(angles_cos(:, 2).*radiuspairs(:, 2)*radius)',...
                          (angles_sin(:, 1).*radiuspairs(:, 1)*radius)',(angles_sin(:, 2).*radiuspairs(:, 2)*radius)'];%ת��ֱ������
end

function [radiuspairs, anglepairs] = getproposals(num_features,radius_grid,angles_grid)

num_radius = length(radius_grid);%�뾶����
num_angles = length(angles_grid);%�Ƕȸ���
%num_radius*num_angles���Ƕ���뾶�������
Pro_a = randperm(num_radius*num_angles);%���������������
Pro_b = randperm(num_radius*num_angles);

exc = Pro_a == Pro_b;%�ж�a,b��Ӧλ���Ƿ����

Pro_a = Pro_a(exc == 0);%�޳�a=b���������
Pro_b = Pro_b(exc == 0);

Pro_a_choose = Pro_a(1:num_features);%ѡ��Ҫ���������Ŀ
Pro_b_choose = Pro_b(1:num_features);

id_radius_a = floor((Pro_a_choose - 1)/num_angles) + 1;%��Ӧ�İ뾶id
id_radius_b = floor((Pro_b_choose - 1)/num_angles) + 1;

id_angles_a = mod(Pro_a_choose, num_angles) + 1;%��Ӧ�ĽǶ�id
id_angles_b = mod(Pro_b_choose, num_angles) + 1;

radiuspairs = [radius_grid(id_radius_a) radius_grid(id_radius_b)];%�뾶��
anglepairs  = [angles_grid(id_angles_a) angles_grid(id_angles_b)];%�Ƕȶ�
end
