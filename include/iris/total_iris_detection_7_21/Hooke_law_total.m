function [Radius, Op_x, Op_y] = Hooke_law_total(x0, y0, p_col, p_row, N, Imax, deta, canshu)
% D Ϊ�ܵ�λ�� total_distance
% Op_x��Op_y Ϊ�µ�ƽ���λ�� new_balance_point
% Imax Ϊ���ѭ������
% canshu��ʾ��Բ������Բ���˶���

% ��ʼ��
Op_x = round(x0);       % Բ�����ĵ�  ��Բ��  �з���
Op_y = round(y0);       % Բ������  �з���
k = 1 / N;
C(1) = 0;
C_d  = 100;
Rp = zeros(1, Imax);
Rp(1) = sum(p_col) / N;    % ƽ���뾶����
m = 2;

p_x = zeros(1,N);
p_y = zeros(1,N);
if strcmp(canshu, 'NeiYuan')    
    % �������Ե����ֱ�������еĶ�Ӧ��
    for i = 1 : N
        p_x(i) = x0 + p_col(i) * cos(p_row(i) / 180 * pi);
        p_y(i) = y0 + p_col(i) * sin(p_row(i) / 180 * pi);
    end
elseif strcmp(canshu, 'WaiYuan')
    for i = 1 : N
        if 0<p_row(i) && p_row(i)<24
            thea = 1 + (p_row(i)-1)*2;
            p_x(i) = x0 + p_col(i) * cos(thea / 180 * pi);
            p_y(i) = y0 + p_col(i) * sin(thea / 180 * pi);
        elseif 23<p_row(i) && p_row(i)<201
            thea = 46 + (p_row(i)-24)/2;
            p_x(i) = x0 + p_col(i) * cos(thea / 180 * pi);
            p_y(i) = y0 + p_col(i) * sin(thea / 180 * pi);
        elseif 200<p_row(i) && p_row(i)<247
            thea = 135 + (p_row(i)-201)*2;
            p_x(i) = x0 + p_col(i) * cos(thea / 180 * pi);
            p_y(i) = y0 + p_col(i) * sin(thea / 180 * pi);
        elseif 246<p_row(i) && p_row(i)<424
            thea = 226 + (p_row(i)-247)/2;
            p_x(i) = x0 + p_col(i) * cos(thea / 180 * pi);
            p_y(i) = y0 + p_col(i) * sin(thea / 180 * pi);
        elseif 423<p_row(i) && p_row(i)<447
            thea = 315 + (p_row(i)-424)*2;
            p_x(i) = x0 + p_col(i) * cos(thea / 180 * pi);
            p_y(i) = y0 + p_col(i) * sin(thea / 180 * pi);
        end
    end
end

p_rr = zeros(1, N);
p_aa = zeros(1, N);
while (m < Imax) && (C_d > deta)
    % ��뾶�ͷ���
    for i = 1 : N
        p_rr(i) = sqrt((p_x(i) - Op_x)^2 + (p_y(i) - Op_y)^2); % 4-14
        if (p_x(i) > Op_x) && (p_y(i) >= Op_y)
            p_aa(i) = asin(abs(p_y(i) - Op_y) / p_rr(i));
        end
        if (p_x(i) <= Op_x) && (p_y(i) > Op_y)
            p_aa(i) = asin(abs(p_x(i) - Op_x) / p_rr(i)) + 0.5 * pi;
        end
        if (p_x(i) < Op_x) && (p_y(i) <= Op_y)
            p_aa(i) = asin(abs(p_y(i)-Op_y) / p_rr(i)) + pi;
        end
        if (p_x(i) >= Op_x) && (p_y(i) < Op_y)
            p_aa(i) = asin(abs(p_x(i) - Op_x) / p_rr(i)) + 1.5 * pi;
        end
    end
    Rp(m) = sum(p_rr) / N;
    % ���λ��
    for i = 1:N
        e_x(i) = cos(p_aa(i));
        e_y(i) = sin(p_aa(i));
    end
    for i = 1:N
        d(i) = (-k) * (Rp(m) - p_rr(i));
    end
    % ����λ��
    D = sum(d(i));
    D_x = d .* e_x;
    D_y = d .* e_y;
    sum_x = sum(D_x);
    sum_y = sum(D_y);
    % ���µ�ƽ�����ĵ�
    Op_x = Op_x + sum_x;
    Op_y = Op_y + sum_y;
    C(m) = abs(D) + abs(Rp(m) - Rp(m-1));
    C_d = abs(C(m) - C(m-1));
    m = m + 1;
end
Op_x = round(Op_x);
Op_y = round(Op_y);
Radius = Rp(m-1);