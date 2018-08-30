clc;
load train_model_new.mat
f = fopen('train_model_new.dat', 'wb');
npt = size(meanshape, 2)/2; %68
max_stage = 5;%�������׶���
fwrite(f, npt, 'integer*4');
fwrite(f, max_stage, 'integer*4');
fwrite(f, NumOfTree, 'integer*4');
fwrite(f, max_depth, 'integer*4');
% ������汾 lbf_model_load �� lbf_face_alignment �õ� dat�ļ�
if 1
    fwrite(f, meanshape, 'double');
    for i=[1: max_stage],
        for j=[1:npt],
            for k=[1:NumOfTree],
                tt = forestmodel{i}(j, k);
                fwrite(f, tt{1}.feat', 'double');
                fwrite(f, tt{1}.thresh, 'double');
            end
        end
    end
    for i=[1: max_stage],
        fwrite(f, Wt{i}, 'double');
    end
end
% ������汾 lbf_model_load �õ� dat�ļ�
if 1
end
fclose(f);

