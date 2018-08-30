clc ; clear; close all;
max_stage = 5;%�������׶���
NumOfTree=1;%ɭ��������
max_depth=5;%�������
TrDataAugment=1;%ѵ���������󣬷�ֹ������ϼ���ø��÷�������
num_features=500;%������
num_points = 68;%��������
Radius = [0.4 0.3 0.2 0.15 0.1];%�趨�뾶
dbnames={'AFW','LFPW','Helen'};%ѡ����Ҫѵ�������ݿ�
work_path=pwd;

disp('step1:��pts��ͼƬ');
%[imgTr,realshapes,imgpathlists]=resizeimg(dbnames,'train');
[imgTr,realshapes,imgpathlists]=readimg(dbnames,'train',num_points);%imgTr��ѵ��������realshapes����ʵ��������״��imgpathlists�����·��
num_ptsfiles=size(realshapes,1);%pts����
num_samples=size(imgTr,2); %ѵ����������

disp('step2:��ƽ����״');
meanshape=sum(realshapes)./num_ptsfiles;%ƽ����״

shape_ref =repmat(realshapes,TrDataAugment,1);%������ʵ��������״
num_samples_Aug=num_samples*TrDataAugment;%������������
clear realshapes;%�������

%���ÿ������ͫ�׼��(��inter-pupil dist)
for i_sample=1:num_samples
    shape_gt=shape_ref(i_sample,:);%��ǰ����i����״
    shape_gt=reshape(shape_gt,num_points,2);%ת��68��2��
    dist_pupils(i_sample,1)=norm((mean(shape_gt(37:42, :)) - mean(shape_gt(43:48, :))));%����iͫ�׼��
end
dist_pupils=repmat(dist_pupils,TrDataAugment,1);

disp('step3:ȷ����ʼ��״');
%{
%V_J���
for i_sample = 1:num_samples
[x1_f,y1_f,x3_f,y3_f] = Viola_Jones_face_detect(work_path,imgTr{i_sample},imgpathlists{i_sample}); %VJ�������λ��
if isempty(x1_f),fprintf('%d\n',i_sample);continue;end
detec_bbox(i_sample,:)=[x1_f,x3_f,y1_f,y3_f];
end
%}
load train_detec_bbox.mat %����������V_J������������������ֻѵ��һ���⣬���������V_J������
if TrDataAugment>1,detec_bbox =repmat(detec_bbox,TrDataAugment,1);end %��������
shape_curstage=randomshape(shape_ref,detec_bbox,num_samples);%��ʼ��״

for i_stage = 1:max_stage
    disp('step4:ȷ����״ƫ��');
    %DeltaS:��״ƫ�Deltas����һ������״ƫ�Delta���ع����״����
    DeltaS=shape_ref-shape_curstage;%��״ƫ���ʵ��״-��ǰ��״
    width=max(shape_curstage(:,1:num_points)')-min(shape_curstage(:,1:num_points)');%������
    height=max(shape_curstage(:,num_points+1:end)')-min(shape_curstage(:,num_points+1:end)');%������
    Deltas=[DeltaS(:,1:num_points)./repmat(width',1,num_points),DeltaS(:,num_points+1:end)./repmat(height',1,num_points)];%����Ӧ�����ߴ磬��״ƫ������һ��
    %Deltas=[bsxfun(@rdivide,DeltaS(:,1:num_points)',width);bsxfun(@rdivide,DeltaS(:,num_points+1:end)',height)]';%��״ƫ������һ��

    radius=Radius(i_stage);%��ǰ�׶ΰ뾶
    disp('step5:����ɭ�֣���ع��������״����');

    %����ɭ��ģ��
    %Delta:��״����;W:�ع����;
    [Delta,W,Forest_Model]=Get_Forest_Model(radius,imgTr,shape_curstage,num_points,num_samples_Aug,NumOfTree,max_depth,Deltas,num_samples,num_features);
    forestmodel{i_stage}=Forest_Model;
    Wt{i_stage}=W;
    Delta=[Delta(:,1:num_points).*repmat(width',1,num_points),Delta(:,num_points+1:end).*repmat(height',1,num_points)];%����Ӧ�����ߴ磬��״ƫ�ԭ
    %Delta=[bsxfun(@times,Delta(:,1:num_points)',width);bsxfun(@times,Delta(:,num_points+1:end)',height)]';
    shape_curstage=shape_curstage+Delta;

    %�������
    deltashapes=DeltaS-Delta;%�ع�����״ƫ��
    deltashapes_x=deltashapes(:,1:num_points);%x����ƫ��
    deltashapes_y=deltashapes(:,num_points+1:end);%y����ƫ��
    error_per_image=mean(sqrt(deltashapes_x.^2+deltashapes_y.^2), 2)./dist_pupils;%����ͼƬ�ľ��������
    MRSE = 100*mean(error_per_image);%����ͼƬ�ľ��������
    MRSE_display = sprintf('Mean Root Square Error for %d Training Samples: %f', num_samples_Aug, MRSE);disp(MRSE_display);%��ʾ���������
end
save('train_model_new.mat','forestmodel','Wt','meanshape','NumOfTree','max_depth');
