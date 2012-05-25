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

set xtics ("1" 1, "2" 2, "4" 4, "8" 8, "16" 16)

plot 'hera-slice-block-100.results' using 1:2 title 'Vector-Tensor Product (100)', \
     'hera-slice-block-200.results' using 1:2 title 'Vector-Tensor Product (200)', \
     'hera-slice-block-300.results' using 1:2 title 'Vector-Tensor Product (300)', \
     'hera-slice-block-400.results' using 1:2 title 'Vector-Tensor Product (400)', \
     'hera-slice-block-500.results' using 1:2 title 'Vector-Tensor Product (500)', \
     'hera-slice-block-600.results' using 1:2 title 'Vector-Tensor Product (600)', \
     'hera-slice-block-700.results' using 1:2 title 'Vector-Tensor Product (700)', \
     'hera-slice-block-800.results' using 1:2 title 'Vector-Tensor Product (800)', \
     'hera-slice-block-900.results' using 1:2 title 'Vector-Tensor Product (900)', \
     'hera-slice-block-1000.results' using 1:2 title 'Vector-Tensor Product (1000)'
