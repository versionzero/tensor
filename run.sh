#!/bin/bash

LINE=${1:-64}
CACHE=${2:-2048}

declare -a STRATEGIES=('compressed' 'compressed' 'slice' 'slice' 'slice' 'ekmr')
declare -a ORIENTATIONS=('row' 'tube' 'lateral' 'horizontal' 'frontal' 'row')

for X in 150 200 250 300 350 400; do
    printf "Processing: $X:"
    for Y in {1..5}; do
	printf " $Y "
	for i in {0..4}; do
	    #printf " ${STRATEGY} "
	    FILE="tensor.$X.$Y.tsr.${STRATEGIES[$i]}.${ORIENTATIONS[$i]}"
	    TENSOR="${FILE}.in"
	    RESULTS="${FILE}.results"
	    ./tensor effectuate -l ${LINE} -m ${CACHE} -o n-mode -s input/vector$X.in intermediate/${TENSOR} >results/${RESULTS} 2>&1
	    printf "."
	done
    done
    printf " [done].\n"
done
