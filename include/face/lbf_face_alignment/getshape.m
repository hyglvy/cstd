function realshape=getshape(pic_path)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%�������ܣ�
%���룺
%pic_path����������·��
%�����
%realshape��������ʵ��״
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%��ȡ������������״
shapepath = strcat(pic_path(1:end-3), 'pts');%������Ӧ��pts·��
file = fopen(shapepath);
pts = textscan(file, '%f %f', 'HeaderLines', 3, 'CollectOutput', 2);%��ȡ�ı�����
realshape=pts{1};%������״
fclose(file);%�ر��ļ�
end