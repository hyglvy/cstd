function [Delta,Wt,Forest_Model]=Get_Forest_Model(radius,imgTr,shape_curstage,num_points,num_samples_Aug,NumOfTree,max_depth,Deltas,num_samples,num_features)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%�������ܣ�����ɭ��ģ��
%���룺
%radius���ֲ�����뾶
% imgTr:��������
%shape_curstage����ǰ��״
%num_points����������Ŀ
%num_samples_Aug�������������Ŀ
%NumOfTree��������Ŀ
%max_depth��������
%Deltas����ʵ��״ƫ��
%num_samples��������Ŀ
%num_features��������Ŀ
%�����
%Delta��Ԥ�����״����
%Wt���ع����
%Forest_Model��ɭ��ģ��
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    forests=cell(num_points,NumOfTree);%ɭ�ֳ�ʼ��
    BF=[];  %��ֵ������ʼ��
    for i_point = 1:num_points
        LBF=[];%�ֲ���ֵ������ʼ��
        forest=cell(1,NumOfTree);
        tic;
        for i_tree=1:NumOfTree
            
            %%%%%%%%%%%%%%������ʼ%%%%%%%%%%%%%%%%%%%%%
            %
            %���ѡ�񲿷�������ȡ1/10
            randsample=randperm(num_samples_Aug);
            idsample=randsample(1:ceil(num_samples_Aug/10));
            %}
            [pixel_diff_features,pixelpair]=Get_diff_features(radius,imgTr,shape_curstage,idsample,i_point,num_samples,num_features); %pixel_diff_features�����ز�������pixelpair��������Ӧ�����ص�
            Deltap=[Deltas(idsample,i_point) Deltas(idsample,i_point+num_points)];%�����ڵ�i_point��ƫ�
            rt=regtree(Deltap,max_depth,pixel_diff_features,pixelpair); %�õ���ģ��
            %%%%%%%%%%%%%%�������%%%%%%%%%%%%%%%%%%%%%
            
            forest{i_tree}=rt;%����n��������ɭ��
            %�ֲ���ֵ����
            bincode=get_lbf(rt,num_samples,num_samples_Aug,radius,imgTr,shape_curstage,i_point,num_points,max_depth);
            LBF=[LBF,bincode];%����������i_point�ľֲ�����
        end
        toc;
        forests(i_point,:)=forest;%����68����ɭ��
        BF=[BF,LBF];%�������оֲ�����
    end
    %toc;
    Forest_Model=forests;
    Wt=get_wt(BF,Deltas);%ȫ�ֻع����
    Delta=BF*Wt;%�õ���״����
end

function  bincode=get_lbf(rt,num_samples,num_samples_Aug,radius,imgTr,shape_curstage,i_point,num_points,max_depth)
          bincode=[];
          for ii=1:2^(max_depth-1)-1
          rt.cnodes(ii,:)=[2*ii,2*ii+1];
          end
          rt.id_leafnodes=2^(max_depth-1):2^max_depth-1;rt.num_leafnodes=2^(max_depth-1);
          for i_sample = 1:num_samples_Aug%��ÿһ��ѵ������
              i_Tr=i_sample-num_samples*(ceil(i_sample/num_samples)-1);
              x0=shape_curstage(i_sample,i_point); y0=shape_curstage(i_sample,i_point+num_points);%����i_sample�ڵ�i_point������
              bbox3=max(shape_curstage(i_sample,1:num_points))-min(shape_curstage(i_sample,1:num_points));%����i_sample��������
              bbox3=bbox3*radius;
              bbox4=max(shape_curstage(i_sample,1+num_points:end))-min(shape_curstage(i_sample,1+num_points:end));%����i_sample��������
              bbox4=radius*bbox4;
              [height,width]=size(imgTr{i_Tr});%����i_sample�����
              %%������
              id_cnode=1;%������ʼʱ�Ľ��id
              while(1)
                    if id_cnode>=2^(max_depth-1)%�жϵ�ǰ����Ƿ���Ҷ���
                        j=find(rt.id_leafnodes==id_cnode);break;%�����Ҷ�ڵ㣬�ҳ����ǵڼ���Ҷ�ڵ㣬������ѭ�����������
                    end
                    feat=rt.feat(id_cnode,:);%��ǰ����е������������뾶���Ƕȣ�
                    ax=round(feat(1)*bbox3+x0);%���ص�a��x����ֵ
                    bx=round(feat(2)*bbox3+x0);%���ص�b��x����ֵ
                    ay=round(feat(3)*bbox4+y0);%���ص�a��y����ֵ
                    by=round(feat(4)*bbox4+y0);%���ص�b��y����ֵ
                    ax = max(1, min(ax, width));%��������Լ�������ܳ���ͼƬ���귶Χ
                    ay = max(1, min(ay, height));
                    bx = max(1, min(bx, width));
                    by = max(1, min(by, height));
                    pdfeat=int16(imgTr{i_Tr}(ay,ax))-int16(imgTr{i_Tr}(by,bx));%�õ����ز�����
                    cind = (pdfeat>rt.thresh(id_cnode)) + 1;%�ж����������뵱ǰ�������㻹���ҽ�㣬����Ϊ�ҽ�㣬С��Ϊ����
                    id_cnode = rt.cnodes(id_cnode,cind);%�õ���ǰ����ӽ���id.
              end   
                qt=sparse(1,j,1,1,rt.num_leafnodes);%ϡ��ľֲ���ֵ����
                bincode=[bincode;qt];
          end
end

function Wt=get_wt(BF,Deltas)
param = sprintf('-s 12 -p 0 -c %f -q heart_scale', 1/(size(BF, 1)));%�趨����
W_liblinear = zeros(size(BF, 2), size(Deltas, 2));%�ع�����ʼ��

for o = 1:size(Deltas, 2)
    model = train(Deltas(:, o), BF, param);%�õ�ģ��
    W_liblinear(:, o) = model.w';
end

Wt = W_liblinear;
end

