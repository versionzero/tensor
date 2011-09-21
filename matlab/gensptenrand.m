
for i = 50:50:200
    name = sprintf('./example%d.in', i);
    disp(name);
    fd = fopen(name, 'wt');
    T = sptenrand([i i i], 0.4);
    tensor_write(fd, T);
    fclose(fd);
end