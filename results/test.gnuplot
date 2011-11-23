set term aqua
set logscale x;
set logscale y;
set xlabel  "Cache Size (bits)" 
set ylabel  "Cache Line Size (bits)"
set zlabel  "Cache Miss\nPercent of Memory Accesses"
set cblabel "Cache Miss\nPercent of Memory Accesses"
set xtics (32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384) rotate by 45
set ytics (8, 16, 32, 64, 128, 256, 512, 1024)
set nokey
set dgrid3d 36, 36
set pm3d map
set contour
set grid layerdefault
set size square
set palette gray negative #rgbformulae 21, 22, 23
#set multiplot
#dx = 1.0/2;
#dy = 1.0/5;
#set size dx, dy;
set term postscript eps enhanced monochrome
set output "example100-compressed-row-P.eps"
set title "example100-compressed-row-P"
splot "example100.compressed.row.P.dat" u 2:3:(100*$23) w pm3d
set term aqua
set logscale x;
set logscale y;
set xlabel  "Cache Size (bits)" 
set ylabel  "Cache Line Size (bits)"
set zlabel  "Cache Miss\nPercent of Memory Accesses"
set cblabel "Cache Miss\nPercent of Memory Accesses"
set xtics (32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384) rotate by 45
set ytics (8, 16, 32, 64, 128, 256, 512, 1024)
set nokey
set dgrid3d 36, 36
set pm3d map
set contour
set grid layerdefault
set size square
set palette gray negative #rgbformulae 21, 22, 23
#set multiplot
#dx = 1.0/2;
#dy = 1.0/5;
#set size dx, dy;
set term postscript eps enhanced monochrome
set output "example100-compressed-row-T.eps"
set title "example100-compressed-row-T"
splot "example100.compressed.row.T.dat" u 2:3:(100*$23) w pm3d
set term aqua
set logscale x;
set logscale y;
set xlabel  "Cache Size (bits)" 
set ylabel  "Cache Line Size (bits)"
set zlabel  "Cache Miss\nPercent of Memory Accesses"
set cblabel "Cache Miss\nPercent of Memory Accesses"
set xtics (32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384) rotate by 45
set ytics (8, 16, 32, 64, 128, 256, 512, 1024)
set nokey
set dgrid3d 36, 36
set pm3d map
set contour
set grid layerdefault
set size square
set palette gray negative #rgbformulae 21, 22, 23
#set multiplot
#dx = 1.0/2;
#dy = 1.0/5;
#set size dx, dy;
set term postscript eps enhanced monochrome
set output "example200-compressed-row-P.eps"
set title "example200-compressed-row-P"
splot "example200.compressed.row.P.dat" u 2:3:(100*$23) w pm3d
set term aqua
set logscale x;
set logscale y;
set xlabel  "Cache Size (bits)" 
set ylabel  "Cache Line Size (bits)"
set zlabel  "Cache Miss\nPercent of Memory Accesses"
set cblabel "Cache Miss\nPercent of Memory Accesses"
set xtics (32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384) rotate by 45
set ytics (8, 16, 32, 64, 128, 256, 512, 1024)
set nokey
set dgrid3d 36, 36
set pm3d map
set contour
set grid layerdefault
set size square
set palette gray negative #rgbformulae 21, 22, 23
#set multiplot
#dx = 1.0/2;
#dy = 1.0/5;
#set size dx, dy;
set term postscript eps enhanced monochrome
set output "example200-compressed-row-T.eps"
set title "example200-compressed-row-T"
splot "example200.compressed.row.T.dat" u 2:3:(100*$23) w pm3d
set term aqua
set logscale x;
set logscale y;
set xlabel  "Cache Size (bits)" 
set ylabel  "Cache Line Size (bits)"
set zlabel  "Cache Miss\nPercent of Memory Accesses"
set cblabel "Cache Miss\nPercent of Memory Accesses"
set xtics (32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384) rotate by 45
set ytics (8, 16, 32, 64, 128, 256, 512, 1024)
set nokey
set dgrid3d 36, 36
set pm3d map
set contour
set grid layerdefault
set size square
set palette gray negative #rgbformulae 21, 22, 23
#set multiplot
#dx = 1.0/2;
#dy = 1.0/5;
#set size dx, dy;
set term postscript eps enhanced monochrome
set output "example100-compressed-tube-P.eps"
set title "example100-compressed-tube-P"
splot "example100.compressed.tube.P.dat" u 2:3:(100*$23) w pm3d
set term aqua
set logscale x;
set logscale y;
set xlabel  "Cache Size (bits)" 
set ylabel  "Cache Line Size (bits)"
set zlabel  "Cache Miss\nPercent of Memory Accesses"
set cblabel "Cache Miss\nPercent of Memory Accesses"
set xtics (32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384) rotate by 45
set ytics (8, 16, 32, 64, 128, 256, 512, 1024)
set nokey
set dgrid3d 36, 36
set pm3d map
set contour
set grid layerdefault
set size square
set palette gray negative #rgbformulae 21, 22, 23
#set multiplot
#dx = 1.0/2;
#dy = 1.0/5;
#set size dx, dy;
set term postscript eps enhanced monochrome
set output "example100-compressed-tube-T.eps"
set title "example100-compressed-tube-T"
splot "example100.compressed.tube.T.dat" u 2:3:(100*$23) w pm3d
set term aqua
set logscale x;
set logscale y;
set xlabel  "Cache Size (bits)" 
set ylabel  "Cache Line Size (bits)"
set zlabel  "Cache Miss\nPercent of Memory Accesses"
set cblabel "Cache Miss\nPercent of Memory Accesses"
set xtics (32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384) rotate by 45
set ytics (8, 16, 32, 64, 128, 256, 512, 1024)
set nokey
set dgrid3d 36, 36
set pm3d map
set contour
set grid layerdefault
set size square
set palette gray negative #rgbformulae 21, 22, 23
#set multiplot
#dx = 1.0/2;
#dy = 1.0/5;
#set size dx, dy;
set term postscript eps enhanced monochrome
set output "example200-compressed-tube-P.eps"
set title "example200-compressed-tube-P"
splot "example200.compressed.tube.P.dat" u 2:3:(100*$23) w pm3d
set term aqua
set logscale x;
set logscale y;
set xlabel  "Cache Size (bits)" 
set ylabel  "Cache Line Size (bits)"
set zlabel  "Cache Miss\nPercent of Memory Accesses"
set cblabel "Cache Miss\nPercent of Memory Accesses"
set xtics (32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384) rotate by 45
set ytics (8, 16, 32, 64, 128, 256, 512, 1024)
set nokey
set dgrid3d 36, 36
set pm3d map
set contour
set grid layerdefault
set size square
set palette gray negative #rgbformulae 21, 22, 23
#set multiplot
#dx = 1.0/2;
#dy = 1.0/5;
#set size dx, dy;
set term postscript eps enhanced monochrome
set output "example200-compressed-tube-T.eps"
set title "example200-compressed-tube-T"
splot "example200.compressed.tube.T.dat" u 2:3:(100*$23) w pm3d
set term aqua
set logscale x;
set logscale y;
set xlabel  "Cache Size (bits)" 
set ylabel  "Cache Line Size (bits)"
set zlabel  "Cache Miss\nPercent of Memory Accesses"
set cblabel "Cache Miss\nPercent of Memory Accesses"
set xtics (32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384) rotate by 45
set ytics (8, 16, 32, 64, 128, 256, 512, 1024)
set nokey
set dgrid3d 36, 36
set pm3d map
set contour
set grid layerdefault
set size square
set palette gray negative #rgbformulae 21, 22, 23
#set multiplot
#dx = 1.0/2;
#dy = 1.0/5;
#set size dx, dy;
set term postscript eps enhanced monochrome
set output "example100-slice-lateral-P.eps"
set title "example100-slice-lateral-P"
splot "example100.slice.lateral.P.dat" u 2:3:(100*$23) w pm3d
set term aqua
set logscale x;
set logscale y;
set xlabel  "Cache Size (bits)" 
set ylabel  "Cache Line Size (bits)"
set zlabel  "Cache Miss\nPercent of Memory Accesses"
set cblabel "Cache Miss\nPercent of Memory Accesses"
set xtics (32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384) rotate by 45
set ytics (8, 16, 32, 64, 128, 256, 512, 1024)
set nokey
set dgrid3d 36, 36
set pm3d map
set contour
set grid layerdefault
set size square
set palette gray negative #rgbformulae 21, 22, 23
#set multiplot
#dx = 1.0/2;
#dy = 1.0/5;
#set size dx, dy;
set term postscript eps enhanced monochrome
set output "example100-slice-lateral-T.eps"
set title "example100-slice-lateral-T"
splot "example100.slice.lateral.T.dat" u 2:3:(100*$23) w pm3d
set term aqua
set logscale x;
set logscale y;
set xlabel  "Cache Size (bits)" 
set ylabel  "Cache Line Size (bits)"
set zlabel  "Cache Miss\nPercent of Memory Accesses"
set cblabel "Cache Miss\nPercent of Memory Accesses"
set xtics (32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384) rotate by 45
set ytics (8, 16, 32, 64, 128, 256, 512, 1024)
set nokey
set dgrid3d 36, 36
set pm3d map
set contour
set grid layerdefault
set size square
set palette gray negative #rgbformulae 21, 22, 23
#set multiplot
#dx = 1.0/2;
#dy = 1.0/5;
#set size dx, dy;
set term postscript eps enhanced monochrome
set output "example200-slice-lateral-P.eps"
set title "example200-slice-lateral-P"
splot "example200.slice.lateral.P.dat" u 2:3:(100*$23) w pm3d
set term aqua
set logscale x;
set logscale y;
set xlabel  "Cache Size (bits)" 
set ylabel  "Cache Line Size (bits)"
set zlabel  "Cache Miss\nPercent of Memory Accesses"
set cblabel "Cache Miss\nPercent of Memory Accesses"
set xtics (32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384) rotate by 45
set ytics (8, 16, 32, 64, 128, 256, 512, 1024)
set nokey
set dgrid3d 36, 36
set pm3d map
set contour
set grid layerdefault
set size square
set palette gray negative #rgbformulae 21, 22, 23
#set multiplot
#dx = 1.0/2;
#dy = 1.0/5;
#set size dx, dy;
set term postscript eps enhanced monochrome
set output "example200-slice-lateral-T.eps"
set title "example200-slice-lateral-T"
splot "example200.slice.lateral.T.dat" u 2:3:(100*$23) w pm3d
set term aqua
set logscale x;
set logscale y;
set xlabel  "Cache Size (bits)" 
set ylabel  "Cache Line Size (bits)"
set zlabel  "Cache Miss\nPercent of Memory Accesses"
set cblabel "Cache Miss\nPercent of Memory Accesses"
set xtics (32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384) rotate by 45
set ytics (8, 16, 32, 64, 128, 256, 512, 1024)
set nokey
set dgrid3d 36, 36
set pm3d map
set contour
set grid layerdefault
set size square
set palette gray negative #rgbformulae 21, 22, 23
#set multiplot
#dx = 1.0/2;
#dy = 1.0/5;
#set size dx, dy;
set term postscript eps enhanced monochrome
set output "example100-slice-horizontal-P.eps"
set title "example100-slice-horizontal-P"
splot "example100.slice.horizontal.P.dat" u 2:3:(100*$23) w pm3d
set term aqua
set logscale x;
set logscale y;
set xlabel  "Cache Size (bits)" 
set ylabel  "Cache Line Size (bits)"
set zlabel  "Cache Miss\nPercent of Memory Accesses"
set cblabel "Cache Miss\nPercent of Memory Accesses"
set xtics (32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384) rotate by 45
set ytics (8, 16, 32, 64, 128, 256, 512, 1024)
set nokey
set dgrid3d 36, 36
set pm3d map
set contour
set grid layerdefault
set size square
set palette gray negative #rgbformulae 21, 22, 23
#set multiplot
#dx = 1.0/2;
#dy = 1.0/5;
#set size dx, dy;
set term postscript eps enhanced monochrome
set output "example100-slice-horizontal-T.eps"
set title "example100-slice-horizontal-T"
splot "example100.slice.horizontal.T.dat" u 2:3:(100*$23) w pm3d
set term aqua
set logscale x;
set logscale y;
set xlabel  "Cache Size (bits)" 
set ylabel  "Cache Line Size (bits)"
set zlabel  "Cache Miss\nPercent of Memory Accesses"
set cblabel "Cache Miss\nPercent of Memory Accesses"
set xtics (32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384) rotate by 45
set ytics (8, 16, 32, 64, 128, 256, 512, 1024)
set nokey
set dgrid3d 36, 36
set pm3d map
set contour
set grid layerdefault
set size square
set palette gray negative #rgbformulae 21, 22, 23
#set multiplot
#dx = 1.0/2;
#dy = 1.0/5;
#set size dx, dy;
set term postscript eps enhanced monochrome
set output "example200-slice-horizontal-P.eps"
set title "example200-slice-horizontal-P"
splot "example200.slice.horizontal.P.dat" u 2:3:(100*$23) w pm3d
set term aqua
set logscale x;
set logscale y;
set xlabel  "Cache Size (bits)" 
set ylabel  "Cache Line Size (bits)"
set zlabel  "Cache Miss\nPercent of Memory Accesses"
set cblabel "Cache Miss\nPercent of Memory Accesses"
set xtics (32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384) rotate by 45
set ytics (8, 16, 32, 64, 128, 256, 512, 1024)
set nokey
set dgrid3d 36, 36
set pm3d map
set contour
set grid layerdefault
set size square
set palette gray negative #rgbformulae 21, 22, 23
#set multiplot
#dx = 1.0/2;
#dy = 1.0/5;
#set size dx, dy;
set term postscript eps enhanced monochrome
set output "example200-slice-horizontal-T.eps"
set title "example200-slice-horizontal-T"
splot "example200.slice.horizontal.T.dat" u 2:3:(100*$23) w pm3d
set term aqua
set logscale x;
set logscale y;
set xlabel  "Cache Size (bits)" 
set ylabel  "Cache Line Size (bits)"
set zlabel  "Cache Miss\nPercent of Memory Accesses"
set cblabel "Cache Miss\nPercent of Memory Accesses"
set xtics (32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384) rotate by 45
set ytics (8, 16, 32, 64, 128, 256, 512, 1024)
set nokey
set dgrid3d 36, 36
set pm3d map
set contour
set grid layerdefault
set size square
set palette gray negative #rgbformulae 21, 22, 23
#set multiplot
#dx = 1.0/2;
#dy = 1.0/5;
#set size dx, dy;
set term postscript eps enhanced monochrome
set output "example100-slice-frontal-P.eps"
set title "example100-slice-frontal-P"
splot "example100.slice.frontal.P.dat" u 2:3:(100*$23) w pm3d
set term aqua
set logscale x;
set logscale y;
set xlabel  "Cache Size (bits)" 
set ylabel  "Cache Line Size (bits)"
set zlabel  "Cache Miss\nPercent of Memory Accesses"
set cblabel "Cache Miss\nPercent of Memory Accesses"
set xtics (32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384) rotate by 45
set ytics (8, 16, 32, 64, 128, 256, 512, 1024)
set nokey
set dgrid3d 36, 36
set pm3d map
set contour
set grid layerdefault
set size square
set palette gray negative #rgbformulae 21, 22, 23
#set multiplot
#dx = 1.0/2;
#dy = 1.0/5;
#set size dx, dy;
set term postscript eps enhanced monochrome
set output "example100-slice-frontal-T.eps"
set title "example100-slice-frontal-T"
splot "example100.slice.frontal.T.dat" u 2:3:(100*$23) w pm3d
set term aqua
set logscale x;
set logscale y;
set xlabel  "Cache Size (bits)" 
set ylabel  "Cache Line Size (bits)"
set zlabel  "Cache Miss\nPercent of Memory Accesses"
set cblabel "Cache Miss\nPercent of Memory Accesses"
set xtics (32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384) rotate by 45
set ytics (8, 16, 32, 64, 128, 256, 512, 1024)
set nokey
set dgrid3d 36, 36
set pm3d map
set contour
set grid layerdefault
set size square
set palette gray negative #rgbformulae 21, 22, 23
#set multiplot
#dx = 1.0/2;
#dy = 1.0/5;
#set size dx, dy;
set term postscript eps enhanced monochrome
set output "example200-slice-frontal-P.eps"
set title "example200-slice-frontal-P"
splot "example200.slice.frontal.P.dat" u 2:3:(100*$23) w pm3d
set term aqua
set logscale x;
set logscale y;
set xlabel  "Cache Size (bits)" 
set ylabel  "Cache Line Size (bits)"
set zlabel  "Cache Miss\nPercent of Memory Accesses"
set cblabel "Cache Miss\nPercent of Memory Accesses"
set xtics (32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384) rotate by 45
set ytics (8, 16, 32, 64, 128, 256, 512, 1024)
set nokey
set dgrid3d 36, 36
set pm3d map
set contour
set grid layerdefault
set size square
set palette gray negative #rgbformulae 21, 22, 23
#set multiplot
#dx = 1.0/2;
#dy = 1.0/5;
#set size dx, dy;
set term postscript eps enhanced monochrome
set output "example200-slice-frontal-T.eps"
set title "example200-slice-frontal-T"
splot "example200.slice.frontal.T.dat" u 2:3:(100*$23) w pm3d
