set term postscript enhanced
set output "mode-1-400.eps"
#set term aqua

#set style line 1 lt 1
#set style line 2 lt 2

set style data linespoints

unset logscale; 
set logscale x;
#set yrange [1:16];

set key right bottom

set title "Matrix-Vector Product (T^{[400]})"
set ylabel "Speedup Improvement"
set xlabel "Thread Count"

set xtics ("1" 1, "2" 2, "4" 4, "8" 8, "16" 16)

plot 'hera-mode-1-row-fiber-400.data' using 1:2 title 'fiber, row', \
     'hera-mode-1-column-fiber-400.data' using 1:2 title 'fiber, column', \
     'hera-mode-1-tube-fiber-400.data' using 1:2 title 'fiber, tube', \
     'hera-mode-1-row-slice-400.data' using 1:2 title 'slice, row', \
     'hera-mode-1-column-slice-400.data' using 1:2 title 'slice, column', \
     'hera-mode-1-tube-slice-400.data' using 1:2 title 'slice, tube'
