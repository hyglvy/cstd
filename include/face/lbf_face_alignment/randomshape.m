function shape_curstage=randomshape(shape_ref,detec_bbox,num_samples)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%�������ܣ������״��ʼ��
%���룺
%shape_ref:��ʵ��������״
%detec_bbox�����������������
%num_samples����������
%�����
%shape_curstage����ʼ�������״
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
num=size(shape_ref,1);
for i=1:num
    randnum=randint(1,1,[1,num_samples]);%���������һ����״
    while(randnum==mod(i,num_samples))%�������������ڱ����ظ�ִ��
        randnum=randint(1,1,[1,num_samples]);
    end
    randshape=shape_ref(randnum,:);%�õ������״
    shape_curstage(i,:)=resetshape(randshape,detec_bbox(i,:));%���ü��������������ʼ����״
end
end