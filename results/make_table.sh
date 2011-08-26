#!/bin/bash

Y=1
declare -a STRATEGIES=('compressed' 'compressed' 'slice' 'slice' 'slice' 'ekmr')
declare -a ORIENTATIONS=('row' 'tube' 'lateral' 'horizontal' 'frontal' 'row')

function get_number () {
    cat "$1" | ack -A 0 -B 0 --nocolor "$2" | cut -d':' -f2
}

echo "\begin{table}[!ht]"
echo "\centering"
echo "\begin{tabular}{|c|r|c|c|c|}"
echo "\hline"
echo "Input & Strategy & Data References & Misses & Non-Trivial Misses\\\\"
echo "\hline"

for X in 150 200 250 300 350 400; do
    printf "\multirow{2}*{\$\X^{[$X]}\$} "
    for i in {0..4}; do
	STRATEGY=${STRATEGIES[$i]}
	ORIENTATION=${ORIENTATIONS[$i]}
	#FILE="${STRATEGY}-${ORIENTATION}-$X-$Y.results"
	FILE="${STRATEGY}-${ORIENTATION}-$X-$Y.results"
	FILE="tensor.$X.4.tsr.${STRATEGIES[$i]}.${ORIENTATIONS[$i]}.results"
	printf "& ${STRATEGY}-${ORIENTATION} "
	printf "& `get_number ${FILE} 'accesses:'`"
	printf "& `get_number ${FILE} 'total misses:'`"
	printf "& `get_number ${FILE} 'non-trivial misses:'`"
	echo "\\\\"
	if [ $i -ne 5 ]; then
	    echo "\cline{2-5}"
	fi
    done
    echo "\hline"
done

echo "\end{tabular}"
echo "\caption{The Cache Profiles of Algorithms for the Vector-Tensor Product Based on \$\TMR(3)\$ and \$\EKMR(3)\$.}"
echo "\label{tbl:cache-profile}"
echo "\end{table}"
