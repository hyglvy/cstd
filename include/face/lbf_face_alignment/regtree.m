 function rt=regtree(Deltap,max_depth,dataSet,pixelpair)
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 %�������ܣ�����������
 %���룺
 %Deltap����һ������״ƫ��
 %max_depth��������
 %dataSet����������
 %pixelpair��������Ӧ�ĽǶ��뼫�뾶
 %�����
 %rt����ģ��
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        num_internode=2^max_depth - 1 - 2^(max_depth - 1);
        [m,n] = size(Deltap);
        max_numnodes = 2^max_depth - 1;%�������
        ind_samples = cell(max_numnodes, 1);%����������ı���
        rtissplit     = zeros(max_numnodes, 1);%����Ƿ���з�֧������0Ϊ��֧��1Ϊ����֦
        %rt.pnode       = zeros(max_numnodes, 1);%���ĸ��ڵ�
        rtdepth       = zeros(max_numnodes, 1);%�������
        %rt.cnodes      = zeros(num_internode, 2);%�����ӽ��
        %rt.isleafnode  = zeros(max_numnodes, 1);%����Ƿ���Ҷ���
        rt.feat        = zeros(num_internode, 4);%������������
        rt.thresh      = zeros(num_internode, 1);%������ֵ
        %���ڵ��ʼ�����£�
        ind_samples{1} = 1:m;
        rtissplit(1)     = 0;
        %rt.pnode(1)       = 0;
        rtdepth(1)       = 1;
        %rt.cnodes(1, 1:2) = [0 0];
        %rt.isleafnode(1)  = 1;
        rt.feat(1, :)     = zeros(1, 4);
        rt.thresh(1)      = 0;
        
        num_nodes = 1;%�������ʼ��Ϊ1
        num_leafnodes = 1;%Ҷ�ڵ�����ʼ��Ϊ1
        stop = 0;%ѭ�����Ʋ���
        while(~stop)
            num_nodes_iter = num_nodes;
            num_split = 0;
            for n = 1:num_nodes_iter
                if ~rtissplit(n)
                    if rtdepth(n) == max_depth
                        rtissplit(n) = 1;
                    else
                         %δ�ﵽ����������������²���
                         [thresh, feat, lcind, rcind] = splitnode(ind_samples{n},Deltap,dataSet,pixelpair,rtdepth(n),max_depth);
                        %���ڵ㸳ֵ
                        %rt.feat(n, :)   = feat;
                        rt.feat(n, :)   = [cos(feat(1))*feat(3),cos(feat(2))*feat(4),sin(feat(1))*feat(3),sin(feat(2))*feat(4)];
                        rt.thresh(n)    = thresh;
                        rtissplit(n)   = 1;
                        %rt.cnodes(n, :) = [num_nodes+1 num_nodes+2];
                        %rt.isleafnode(n)   = 0;
                        %�ӽڵ㸳ֵ
                        ind_samples{num_nodes+1} = lcind;
                        rtissplit(num_nodes+1)     = 0;
                        %rt.pnode(num_nodes+1)       = n;
                        rtdepth(num_nodes+1)       = rtdepth(n) + 1;
                        %rt.cnodes(num_nodes+1, :)   = [0 0];
                        %rt.isleafnode(num_nodes+1)     = 1;
                        
                        ind_samples{num_nodes+2} = rcind;
                        rtissplit(num_nodes+2)     = 0;
                        %rt.pnode(num_nodes+2)       = n;
                        rtdepth(num_nodes+2)       = rtdepth(n) + 1;
                        %rt.cnodes(num_nodes+2, :)   = [0 0];
                        %rt.isleafnode(num_nodes+2)  = 1;
                        
                        num_split = num_split + 1;        %���ﵽ����ʱ�����ٽ��д˲���
                        num_leafnodes = num_leafnodes + 1;%Ҷ�ڵ�������
                        num_nodes     = num_nodes + 2;    %���������
                    end
                end
            end
            
            if num_split == 0
                stop = 1;
            else
                %rt.num_leafnodes = num_leafnodes;
                %rt.num_nodes     = num_nodes;           
                %rt.id_leafnodes = find(rt.isleafnode == 1); 
            end            
        end
end

 function [thresh, feat, lcind, rcind] = splitnode(ind_samples,Deltap,dataSet,pixelpair,depth,max_depth)
 %�������ܣ�
 %ȷ�������ֵ������������Լ����ݻ���
 
%�����жϽ����������Ƿ�Ϊ��
if isempty(ind_samples)
    thresh = 0;
    feat = [0 0 0 0];
    rcind = [];
    lcind = [];
    return;
end

    Tr_Data=dataSet(ind_samples,:);%�����������������   
    DeltaS=Deltap(ind_samples,:);
    m = size(Tr_Data);%���ݼ��Ĵ�С 
    bestS = inf;%�����ʼ��,�����
      
    %�ҵ���ѵ�λ�ú����ŵ�ֵ  
    for j = 1:m(:,2)%�õ��������� 
        [datasort,dataindex]=sort(Tr_Data(:,j));    %��j����������������ֵ��С��������
        deltas=DeltaS(dataindex,:);                 %������Ӧ�ĵ�ƫ��
        [norepdata,norepindex]= unique(datasort);   %�޳�������ظ���ֵ
        
        %������������ֵ
        ki=ceil(length(norepindex)*0.25);ko=ceil(length(norepindex)*0.75);
        pp=norepindex(ki:ko);
        i=pp(randint(1,1,[1 length(pp)]));
        indexn=find(i==pp);
        threshn = norepdata(indexn+ki-1);
        %%%��%%%%%
        
        %����Ƕ�ÿ�㼰ÿ������е��������и���ĿԼ��
        lnum = sum(Tr_Data(:,j) <= threshn);
        rnum = sum(Tr_Data(:,j) > threshn);
        if lnum<2^(max_depth-depth)||rnum<2^(max_depth-depth),continue;end;%����ÿ�����������Ŀ
        %%%��%%%%%
        
        %��j�������������ֵ��Ȩ�ط���
        S=[var(deltas(1:i,1))+var(deltas(1:i,2))]*i*length(datasort)^-1+[var(deltas(i+1:end,1))+var(deltas(i+1:end,2))]*(length(datasort)-i)*length(datasort)^-1;
        if S < bestS  
             bestS = S;%��ѷ������¸�ֵ
             fea = j;  %��ѵ�������
             thresh = threshn;%��ѵ���ֵ
        end
        
    end 
    
    lcind = ind_samples(Tr_Data(:,fea) <= thresh);%����
    rcind = ind_samples(Tr_Data(:,fea) > thresh); %�ҽ��
    feat=pixelpair(fea,:);%��������
end
