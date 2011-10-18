
for i = 100:100:1000
    name = sprintf('./example%d.in', i);
    disp(name);
    T = sptenrand([i i i], 0.01);
    disp('Writing...');
    fd = fopen(name, 'wt');
    tensor_write(fd, T);
    fclose(fd);
end