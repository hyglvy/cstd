function [Pt Pt_new]=isample_traverse(forestmodel,Radius,shape_curstage,i_stage,num_points,NumOfTree,imgTe_gray,bbox3,bbox4,max_depth)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
%�������ܣ�������������ɭ��ģ�ͣ���������ĸ�ά��ֵ����
%���룺
%forestmodel��ɭ��ģ��
%Radius��Բ��뾶
%shape_curstage:��ǰ������״
%i_stage����ǰ�׶�
%num_points����������
%NumOfTree��ɭ����������Ŀ
%imgTe_gray�����������ĻҶ�ͼ����
%bbox3,bbox4����ǰ�������
%�����
%Pt����ֵ����
%�׶�-->��-->��
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
radius=Radius(i_stage);
Pt=[];Pt_new=[];
for i_point = 1:num_points
    forest=forestmodel{i_stage}(i_point,:);%��ǰ��ɭ��ģ��
    x0=shape_curstage(i_point); y0=shape_curstage(i_point+num_points);%��ǰ������
    [pt pt_new]=getlbf(forest,radius,NumOfTree,imgTe_gray,x0,y0,bbox3,bbox4,max_depth);%LBF
    Pt=[Pt,pt];%BF 
    Pt_new=[Pt_new,pt_new];
end
end

function [Qt,Qt_new]=getlbf(forest,radius,NumOfTree,imgTe_gray,x0,y0,bbox3,bbox4,max_depth)
    %��ѵ����һ��������ע�͡�
    bbox3=bbox3*radius;bbox4=radius*bbox4;
    [height,width]=size(imgTe_gray);
    Qt=[];Qt_new=[];
     for ii=1:2^(max_depth-1)-1
         cnodes(ii,:)=[2*ii,2*ii+1];
     end
     num_leafnodes=2^(max_depth-1);
     id_leafnodes=2^(max_depth-1):2^max_depth-1;
     
    for i_tree = 1:NumOfTree
        tree=forest{i_tree};
        %%������
        id_cnode=1;
        while(1)
           if id_cnode>=2^(max_depth-1)
              j=find(id_leafnodes==id_cnode);break;
           end
           feat=tree.feat(id_cnode,:);
           ax=round(feat(1)*bbox3+x0);%���ص�a��x����ֵ
           bx=round(feat(2)*bbox3+x0);%���ص�b��x����ֵ
           ay=round(feat(3)*bbox4+y0);%���ص�a��y����ֵ
           by=round(feat(4)*bbox4+y0);%���ص�b��y����ֵ
           ax = max(1, min(ax, width));
           ay = max(1, min(ay, height));
           bx = max(1, min(bx, width));
           by = max(1, min(by, height));
           pdfeat=int16(imgTe_gray(ay,ax))-int16(imgTe_gray(by,bx));
           cind = (pdfeat>tree.thresh(id_cnode)) + 1;
           id_cnode = cnodes(id_cnode,cind);
        end   
        qt=sparse(1,j,1,1,num_leafnodes);
        Qt=[Qt,qt];Qt_new=[Qt_new,j];
    end
end

