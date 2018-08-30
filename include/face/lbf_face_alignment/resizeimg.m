function [img,realshapes,save_path]=resizeimg(dbnames,mode)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%�������ܣ���ͼƬ���������ٳ����ߴ�����
%���룺
%dbnames�����ݿ���
%mode��ģʽ
%�����
%img����������
%realshapes��������ʵ��״
%save_path����������·��
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
num_points=68;
work_path = pwd;
imgpathlists=[];
ifresize=1;%�Ƿ��ͼƬ������С����1Ϊ��
for i=1:length(dbnames)
    imgpathlistfile = strcat(work_path,'\300-w\',mode,'\', dbnames{i}, '\Path_Images.txt');%���ݿ�·���洢�ı�
    imgpathlist = textread(imgpathlistfile, '%s', 'delimiter', '\n');%���ݿ�·��
    imgpathlists=[imgpathlists;imgpathlist];
    fprintf(strcat(num2str(dbnames{i}),':',num2str(length(imgpathlist)),'\n'));%��ʾ�����ݿ����������
end
    %������ʼ��
    num_samples=length(imgpathlists);
    img=cell(1,num_samples);
    realshapes=zeros(num_samples,num_points*2);

for i_sample=1:num_samples
    pic_path = imgpathlists{i_sample};%����·��
    img_rgb = im2uint8(imread(pic_path));%��ȡ����
    realshape=getshape(pic_path);%��ȡ��״
    [height,width,a]=size(img_rgb);%ͼƬ���
    minx=min(realshape(:,1));maxx=max(realshape(:,1));
    miny=min(realshape(:,2));maxy=max(realshape(:,2));
    w=maxx-minx;h=maxy-miny;%��������
    leftx=max(1,floor(minx-w/4));rightx=min(width,floor(maxx+w/4));%���ұ߽縳ֵ��������ӦԼ����
    topy=max(1,floor(miny-h/4));bottomy=min(height,floor(maxy+h/4));%���±߽縳ֵ��������ӦԼ����
    img_color=img_rgb(topy:bottomy,leftx:rightx,:);%��ȡ������
    realshape=[realshape(:,1)-leftx,realshape(:,2)-topy];%��Ӧ����״����Ӧ�仯��
    clear img_rgb;
        %
        %ѵ���������ҶȻ��������������ֲ�ɫͼ
        if strcmp(mode,'train')
        if ndims(img_color)==3, img_color=rgb2gray(img_color);end
        end
        %}
        %
        %��С
     if ifresize
        [height,width,a]=size(img_color);
        size_fac=ceil(height/300);%��С����
        if size_fac==1
            img_inter=img_color;%����
            realshape_inter=realshape;
        else
            img_inter=imresize(img_color,1/size_fac,'bilinear');%��size_fac������С
            realshape_inter=realshape/size_fac;
        end
     else
         img_inter=img_color;%����
         realshape_inter=realshape;
     end
        %}
        %
     img{i_sample}=img_inter;
     realshapes(i_sample,:)=realshape_inter(:);
     save_path{i_sample}=strcat(pic_path(length(work_path)+8:end-8),num2str(i_sample,'%04d'),'.jpg');%����ͼƬ����·��
     
     %
     %����ͼƬ
     imwrite(img_inter,save_path{i_sample});
     %��������
     fp_w=fopen(strcat(save_path{i_sample}(1:end-4),'.pts'),'w');
     fprintf(fp_w,'%s %d\n','version:',1);
     fprintf(fp_w,'%s %d\n','n_points:',num_points);
     fprintf(fp_w,'%s\n','{');
     fprintf(fp_w,'%f %f\n',realshape_inter');
     fprintf(fp_w,'%s','}');
     fclose(fp_w);
     %}
     clear img_color;
     clear img_inter;
end
end