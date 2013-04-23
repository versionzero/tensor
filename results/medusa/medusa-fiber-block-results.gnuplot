#set term postscript eps enhanced
#set output "tensor.eps"
set term aqua

set style line 1 lt 1
set style line 2 lt 2

set style data linespoints

unset logscale; 
set logscale x;
#set yrange [1:16];

set ylabel "Speedup Improvement"
set xlabel "Thread Count"

set xtics ("1" 1, "2" 2, "4" 4, "8" 8, "16" 16, "32" 32)

plot 'medusa-fiber-block-100.results' using 1:2 title 'Vector-Tensor Product (100)', \
     'medusa-fiber-block-200.results' using 1:2 title 'Vector-Tensor Product (200)', \
     'medusa-fiber-block-300.results' using 1:2 title 'Vector-Tensor Product (300)', \
     'medusa-fiber-block-400.results' using 1:2 title 'Vector-Tensor Product (400)', \
     'medusa-fiber-block-500.results' using 1:2 title 'Vector-Tensor Product (500)', \
     'medusa-fiber-block-600.results' using 1:2 title 'Vector-Tensor Product (600)', \
     'medusa-fiber-block-700.results' using 1:2 title 'Vector-Tensor Product (700)', \
     'medusa-fiber-block-800.results' using 1:2 title 'Vector-Tensor Product (800)', \
     'medusa-fiber-block-900.results' using 1:2 title 'Vector-Tensor Product (900)', \
     'medusa-fiber-block-1000.results' using 1:2 title 'Vector-Tensor Product (1000)'