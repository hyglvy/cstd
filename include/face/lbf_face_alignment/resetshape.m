function shape=resetshape(shape_in,ref_location)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%�������ܣ�������������λ�ó�ʼ��������״
%���룺
%shape_in���������״���������ƽ����״��
%ref_location���������⵽��λ��
%�����
%shape����ʼ����������״
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

bbox(1)=ref_location(1);%left_x
bbox(2)=ref_location(3);%top_y
bbox(3)=ref_location(2)-ref_location(1);%width
bbox(4)=ref_location(4)-ref_location(3);%height

num_points = size(shape_in,2)/2;%������������
width = max(shape_in(1:num_points)) - min(shape_in(1:num_points));%������
height= max(shape_in(num_points+1:end)) - min(shape_in(num_points+1:end));%������
%���������״��һ����1��Ȼ��Ŵ��������С������������״����ƽ�ơ�
shape = [bbox(3)/width*(shape_in(1:num_points)-min(shape_in(1:num_points)))+bbox(1),...
         bbox(4)/height*(shape_in(num_points+1:end)-min(shape_in(num_points+1:end)))+bbox(2)];
end
