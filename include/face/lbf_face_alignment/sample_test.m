clc ; clear; %close all;
load train_model_new.mat
work_path = pwd;
max_stage = 5;%�������׶���
Radius = [0.4 0.3 0.2 0.15 0.1];
%dbnames={'Helen','LFPW','IBUG'};%ѡ��Ҫ���Ե����ݿ�
dbnames={'LFPW'};%ѡ��Ҫ���Ե����ݿ�
num_points=size(meanshape,2)/2;max_depth=5;

disp('step1:��pts��ͼƬ');
%[imgTr,realshapes,imgpathlists]=resizeimg(dbnames,'test');
[imgTe,realshapes,imgpathlists]=readimg(dbnames,'test',num_points);%����������ȡ

disp('step2:��״��ʼ��');
num_pic=length(imgTe);
imgTe_gray=cell(1,num_pic);
detec_bbox=zeros(num_pic,4);
for i_sample = 1:num_pic
    img = imgTe{i_sample};
    if ndims(img)==3, imgTe_gray{i_sample}=rgb2gray(img);
    else imgTe_gray{i_sample}=img;end
    realshape=realshapes(i_sample,:);realshape=reshape(realshape,num_points,2);
    %{
    %V_J�������
    [x1_f,y1_f,x3_f,y3_f] = Viola_Jones_face_detect(work_path,imgTe{i_sample},imgpathlists{i_sample}); %�������λ��
    if isempty(x1_f),fprintf('%d\n',i_sample);continue;end
    detec_bbox(i_sample,:)=[x1_f,x3_f,y1_f,y3_f];
    %}
end

%
%��Ϊ�������ݿ⣬�����ô˶δ��룬ֻ���������V_J��⺯��
detec_bbox=[];
for i_db=1:length(dbnames)
    %�������ȱ����V_J�������⵽��������
    if strcmp(dbnames{i_db},'Helen')
        load helen_test_detec_bbox.mat
    elseif strcmp(dbnames{i_db},'LFPW')
        load lfpw_test_detec_bbox.mat
    elseif strcmp(dbnames{i_db},'IBUG')
        load ibug_test_detec_bbox.mat
    else
        error('��ʹ��V_J function����ע�͵��˶δ���!');
    end 
    detec_bbox=[detec_bbox;bbox];
end
%}
for i_sample = 1:num_pic
    shapes_intial(i_sample,:)=resetshape(meanshape,detec_bbox(i_sample,:));%��״��ʼ��
end
disp('step3:ģ�ͻع�');
t0=cputime;
for i_sample = 1:num_pic
    
    shape_curstage=shapes_intial(i_sample,:);
    %��ʾ��ʼ�������״
    %subplot(1,2,1),imshow(imgTe{i_sample});hold on;plot(shape_curstage(1:num_points),shape_curstage(num_points+1:end),'r.');title('��ʼ��״');hold off;
    bbox3=max(shape_curstage(1:num_points))-min(shape_curstage(1:num_points));%��ǰ������
    bbox4=max(shape_curstage(1+num_points:end))-min(shape_curstage(1+num_points:end));%��ǰ������
    realshape=realshapes(i_sample,:);realshape=reshape(realshape,num_points,2);%������ʵ��״
    
    for i_stage = 1:max_stage
        %BF
        [Pt Pt_new]=isample_traverse(forestmodel,Radius,shape_curstage,i_stage,num_points,NumOfTree,imgTe_gray{i_sample},bbox3,bbox4,max_depth);%���������õ�ȫ�ֶ�ֵ����
        %tic;
        Delta=Pt*Wt{i_stage};%�ع���״����
        %toc;
        %{
        tic;
        Delta_new=0;
        for i_Pt=1:length(Pt_new)
          Delta_new=Delta_new+Wt{i_stage}(16*(i_Pt-1)+Pt_new(i_Pt),:);
        end
        toc;
        %}
        Delta=[Delta(1:num_points)*bbox3,Delta(num_points+1:end)*bbox4];%������Ӧ�������ߴ磬��ԭ��״����
        %����������״
        shape_curstage=shape_curstage+Delta;
        shape_curstage=reshape(shape_curstage,num_points,2);
        %���������ߴ�
        minx=min(shape_curstage(:,1));maxx=max(shape_curstage(:,1));
        miny=min(shape_curstage(:,2));maxy=max(shape_curstage(:,2));
        bbox3=maxx-minx;bbox4=maxy-miny;%����
        error_of_image(i_sample,i_stage) = compute_error(realshape, shape_curstage);%�������
        shape_curstage=shape_curstage(:)';%ת��������
    end
    
    errors(i_sample)=error_of_image(i_sample,max_stage);
    
    %fprintf('%f\n',error_of_image(i_sample,max_stage));%��ʾ����ͼƬ�����
    Shape_result=reshape(shape_curstage,num_points,2);
    %��ʾ���յ���״
    %subplot(1,2,2),imshow(imgTe{i_sample});hold on;plot(Shape_result(:,1),Shape_result(:,2),'g.');title('�����״');hold off;pause;
end 
t1=cputime-t0;
    %
    %������ȷ�ʵ�һ�δ���
    factor=0.1;%�趨һ����λ��׼
    precision_rate=length(find(errors<factor))/length(errors)*100;%����˱�׼�µ���ȷ��
    fprintf('%f\n',precision_rate);%��ʾ��ȷ��
    %}

disp('step4:�������');
for i_stage = 1:max_stage
    MRSE = 100*mean(error_of_image(:,i_stage));
    MRSE_display = sprintf('Mean Root Square Error for %d Test Samples: %f',i_sample, MRSE);
    disp(MRSE_display);
end
disp(strcat('�㷨�ٶ�:',num2str(floor(num_pic/t1)),'fps'));    %matlab����ʾ�㷨�ٶ�