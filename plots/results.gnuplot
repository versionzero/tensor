set term postscript eps enhanced
set output "tensor.eps"
#set term aqua

set style line 1 lt 1
set style line 2 lt 2

#set xlabel "Bit Rate" font "Times-Roman,24"; 
#set ylabel "Compression Ratio\n(% of Original Image Size)" font "Times-Roman,24";
#set key on top left# font "Times-Roman,20";

#set xtics 1 font "Times-Roman,16";
#set ytics 0.05 font "Times-Roman,16";
set style data linespoints
#unset logscale; set logscale y; set logscale y2

set y2tics;
set ytics mirror;
set multiplot;
#plot [5:30]
plot \
'compressed.data' using 1:3 title 'Misses for compressed'           lt 1 axis x1y1, \
'compressed.data' using 1:4 title 'Non-Trivial Misses for compressed'          lt 1 axis x1y1,\
'ekmr.data' using 1:3 title 'Misses for ekmr'           lt 2 axis x1y1, \
'ekmr.data' using 1:4 title 'Non-Trivial Misses for ekmr'          lt 2 axis x1y1