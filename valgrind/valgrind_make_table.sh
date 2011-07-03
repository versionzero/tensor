#!/bin/bash

Y=1
ORIENTATION="row"
declare -a STRATEGIES=('compressed' 'ekmr')

function get_number () {
    cat "$1" | ack -A 0 -B 0 --nocolor "$2" | cut -d':' -f2 | cut -d'(' -f1
}

echo "\begin{table}[!ht]"
echo "\centering"
echo "\begin{tabular}{|c|r|c|c|c|}"
echo "\hline"
echo "Input & Strategy & Data References & \$D1\$ misses & \$LLd\$ misses\\\\"
echo "\hline"

for X in 10 50 100 150 300 400; do 
    printf "\multirow{2}*{\$\X^{[$X]}\$} "
    for STRATEGY in ${STRATEGIES[@]}; do
	FILE="${STRATEGY}-${ORIENTATION}-$X-$Y.results"
	printf "& ${STRATEGY} "
	printf "& `get_number ${FILE} 'D   refs:'`"
	printf "& `get_number ${FILE} 'D1  misses:'`"
	printf "& `get_number ${FILE} 'LLd misses:'`"
	echo "\\\\"
	if [ "${STRATEGY}" == "compressed" ]; then
	    echo "\cline{2-5}"
	fi
    done
    echo "\hline"
done

echo "\end{tabular}"
echo "\label{tbl:cache-profile}"
echo "\caption{The Cache Profiles of Algorithms for the Vector-Tensor Product Based on \$\TMR(3)\$ and \$\EKMR(3)\$.}"
echo "\end{table}"
