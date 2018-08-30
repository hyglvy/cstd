function [ error_of_image ] = compute_error(ground_truth_points, detected_points)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%�������ܣ����㵥���������
%���룺
%ground_truth_points����ʵ��״
%detected_points����⵽����״
%�����
%error_of_image�������������
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    num_points = size(ground_truth_points,1);%��������Ŀ
    interocular_distance = norm(mean(ground_truth_points(37:42,:))-mean(ground_truth_points(43:48,:))); %�۾����ľ���
    sum=0;
    for j=1:num_points
        sum = sum+norm(detected_points(j,:)-ground_truth_points(j,:));%��������������
    end
    error_of_image = sum/(num_points*interocular_distance);%����ͼƬ�����

end