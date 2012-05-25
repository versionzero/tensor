#!/bin/bash

function get_number () {
    cat "$1" | ack -A 0 -B 0 --nocolor "$2" | cut -d':' -f2
}

FILENAME="tensor"

printf "Generating gnuplot script '${FILENAME}'\n"

cat >|${FILENAME}.gnu <<EOF
set term postscript eps enhanced
set output "${FILENAME}.eps"
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
EOF

declare -a STRATEGIES=('compressed' 'compressed' 'slice' 'slice' 'slice' 'ekmr')
declare -a ORIENTATIONS=('row' 'tube' 'lateral' 'horizontal' 'frontal' 'row')

FILENAME=${FILENAME}.gnu

#for STRATEGY in ${STRATEGIES[@]}; do
echo "plot \\" >>${FILENAME}
for i in {0..4}; do
    STRATEGY=${STRATEGIES[$i]}
    ORIENTATION=${ORIENTATIONS[$i]}
    NAME="${STRATEGY}.${ORIENTATION}"
    #echo "set key on top ${LOCATION}"                                                                >>${FILENAME}
    echo "'${NAME}.data' using 1:2 title 'Data references for ${NAME}'      lt $((i+1)) axis x1y1, \\" >>${FILENAME}
    echo "'${NAME}.data' using 1:3 title 'Misses for ${NAME}'               lt $((i+1)) axis x1y1, \\" >>${FILENAME}
    printf "'${NAME}.data' using 1:4 title 'Non-Trivial Misses for ${NAME}' lt $((i+1)) axis x1y1"     >>${FILENAME}
    if [ $i -ne 4 ]; then
	echo ",\\" >>${FILENAME}
    fi
done

gnuplot ${FILENAME}
open *.eps

