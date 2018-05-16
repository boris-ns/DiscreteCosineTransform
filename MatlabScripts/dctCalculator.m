% Script calculates DCT. Reads matrices from input files.
% Writes result to files.

% Read input matrices from files
path = '..\tests\matrices15x15\';
in = dlmread(strcat(path, 'in.txt'));
c = dlmread(strcat(path, 'c.txt'));
alpha = dlmread(strcat(path, 'alpha.txt'));

% Calculate DCT
rrr = alpha .* ((in * c) * c');

% Write result of phase 3 to txt file
resultFile = fopen(strcat(path, 'result.txt'), 'wt');

for i = 1 : size(rrr, 1)
    for j = 1 : size(rrr, 1)
        fprintf(resultFile, '%s ', num2str(rrr(i, j), '%.0f'));
    end
end

fclose(resultFile);