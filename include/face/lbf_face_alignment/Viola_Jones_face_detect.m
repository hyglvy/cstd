function  [x1_f,y1_f,x3_f,y3_f] = Viola_Jones_face_detect(work_path,img,pic_path)

if_show_VJface = 0;%�Ƿ���ʾ������������,1Ϊ��ʾ

fp1 = fopen('face_detect\face_detect.bat','w');
fprintf(fp1,'%s\n','@echo off');
path_for_bat = strcat('..',pic_path(length(work_path)+1:end));
fprintf(fp1,'%s %s','face_detect.exe',path_for_bat);
fclose(fp1);

cd(strcat(work_path,'\face_detect'));
system('face_detect.bat');

cd(work_path);

fp2 = fopen('face_detect\facePoints.txt','r');
facePoints = fscanf(fp2,'%f');%��Ϊc++����ϵ
fclose(fp2);
if facePoints(1) == -1,disp('��һ֡û�м�⵽�κ�����');x1_f=[];x3_f=[];y1_f=[];y3_f=[];return;end

x1_f = facePoints(1);y1_f = facePoints(2);%ע��˽������opencv��Viola Jones�ļ����������������һ���ĳ߶����ź����ƺ�õ��Ľ����������ԭʼ������λ�ã����ź����������Լ������opencv��������cpp�ļ��д���ģ���������matlab�д����
x3_f = facePoints(3);y3_f = facePoints(4);

x1_f = x1_f + 1;y1_f = y1_f + 1;x3_f = x3_f + 1;y3_f = y3_f + 1;
x2_f = x3_f;y2_f = y1_f;x4_f = x1_f;y4_f = y3_f;  
if if_show_VJface == 1
    figure,imshow(img)
    hold on
    plot([x1_f;x2_f;x3_f;x4_f;x1_f],[y1_f;y2_f;y3_f;y4_f;y1_f],'r')%���������ο�
    hold off
    pause(0.4)
    close
end