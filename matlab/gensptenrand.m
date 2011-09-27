
for i = 500:50:1000
    name = sprintf('./example%d.in', i);
    disp(name);
    T = sptenrand([i i i], 0.1);
    disp('Writing...');
    fd = fopen(name, 'wt');
    tensor_write(fd, T);
    fclose(fd);
end