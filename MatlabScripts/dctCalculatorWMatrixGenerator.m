% Script generates matrices and calculates DCT. 
% Writes all matrices to files.

MATRIX_DIM = 50;
path = '..\tests\matrices500x500\';

% Generate matrices with random elements
in = randi(256, MATRIX_DIM, MATRIX_DIM);
c = randi(256, MATRIX_DIM, MATRIX_DIM);
alpha = randi(256, MATRIX_DIM, MATRIX_DIM);

% Write matrices to file
dlmwrite(strcat(path, 'in.txt'), in, ' ');
dlmwrite(strcat(path, 'c.txt'), c, ' ');
dlmwrite(strcat(path, 'alpha.txt'), alpha, ' ');

% Calculate DCT
rrr = alpha .* ((in * c) * c');

% Write result to file
resultFile = fopen(strcat(path, 'result.txt'), 'wt');

for i = 1 : size(rrr, 1)
    for j = 1 : size(rrr, 1)
        fprintf(resultFile, '%s ', num2str(rrr(i, j), '%.0f'));
    end
    
    fprintf(resultFile, '\n');
end

fclose(resultFile);
