set term postscript enhanced
set output "mode-2.eps"
#set term aqua grayscale

set style line 1 lt 1
set style line 2 lt 2

set style data linespoints

unset logscale; 
set logscale x;
#set yrange [1:16];

set ylabel "Speedup Improvement"
set xlabel "Thread Count"

set xtics ("1" 1, "2" 2, "4" 4, "8" 8, "16" 16)

plot 'hera-mode-2-200.data' using 1:2 title 'Matrix-Tensor Product (300)', \
     'hera-mode-2-300.data' using 1:2 title 'Matrix-Tensor Product (300)', \
     'hera-mode-2-400.data' using 1:2 title 'Matrix-Tensor Product (400)'
