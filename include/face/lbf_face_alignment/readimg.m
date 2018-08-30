function [img,realshapes,imgpathlists]=readimg(dbnames,mode,num_points)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%�������ܣ���ȡ�������ݣ���״��·��
%���룺
%dbnames�����ݿ���
%mode��ģʽ
%num_points����������
%�����
%img����������
%realshapes��������ʵ��״
%imgpathlists����������·��
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
work_path = pwd;%%��ǰ����·��
imgpathlists=[];%����·����ʼ��
for i=1:length(dbnames)
    imgpathlistfile = strcat(work_path,'\',mode,'\', dbnames{i}, '\Path_Images.txt');%txt·��
    imgpathlist = textread(imgpathlistfile, '%s', 'delimiter', '\n');%��ȡ����·��
    imgpathlists=[imgpathlists;imgpathlist];%�������ݿ�����������
    fprintf(strcat(num2str(dbnames{i}),':',num2str(length(imgpathlist)),'\n'));%��ӡ��ǰ���ݿ����������������п�ɾ����Ҫ��
end
%������ʼ��
num_samples=length(imgpathlists);%��������
img=cell(1,num_samples);
realshapes=zeros(num_samples,num_points*2);

for i_sample=1:num_samples
    pic_path = imgpathlists{i_sample};%����i·��
    img{i_sample} = im2uint8(imread(pic_path));%��ȡ����i���ݲ�ת��8λ�޷�����
    intershape=getshape(pic_path);%��ȡ����i��ʵ��������״
    realshapes(i_sample,:)=intershape(:);%��2�����ݴ��ӳ�һ��    
end
end