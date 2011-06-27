#!/bin/bash

Y=1
ORIENTATION="row"

function get_number () {
    cat "$1" | ack -A 0 -B 0 --nocolor "$2" | cut -d':' -f2 | cut -d'(' -f1 | sed 's/,//g'
}

FILENAME="tensor.gnu"

printf "Generating gnuplot script '${FILENAME}'\n"

cat >tensor.gnu <<EOF
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
unset logscale; set logscale y

set multiplot;
#plot [5:30]
EOF

declare -a STRATEGIES=('compressed' 'ekmr')
declare -a LOCATIONS=('left' 'right')

#for STRATEGY in ${STRATEGIES[@]}; do
for X in {0..1}; do
    STRATEGY=${STRATEGIES[$X]}
    LOCATION=${LOCATIONS[$X]}
    echo "set key on top ${LOCATION}"                                                                >>${FILENAME}
    echo "plot '${STRATEGY}.data' using 1:2 title 'Data references for ${STRATEGY}' lt $((X+1)), \\" >>${FILENAME}
    echo "'${STRATEGY}.data' using 1:3 title 'D1 misses for ${STRATEGY}'            lt $((X+1)), \\" >>${FILENAME}
    echo "'${STRATEGY}.data' using 1:4 title 'LLd misses for ${STRATEGY}'           lt $((X+1))"     >>${FILENAME}
done

printf "Generating data files:"

for STRATEGY in ${STRATEGIES[@]}; do
    rm -f ${STRATEGY}.data
    touch ${STRATEGY}.data
    printf " ${STRATEGY}.data "
    for X in 10 50 100 150 300 400; do 
	FILE="${STRATEGY}-${ORIENTATION}-$X-$Y.results"
	printf "$X `get_number ${FILE} 'D   refs:'` "    >>${STRATEGY}.data
	printf "`get_number ${FILE} 'D1  misses:'` "  >>${STRATEGY}.data
	printf "`get_number ${FILE} 'LLd misses:'`\n" >>${STRATEGY}.data
	printf "."
    done
done

printf " [done]\n"

#gnuplot g.gnu
#rm -f g.gnu

