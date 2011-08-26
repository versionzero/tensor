#!/bin/bash

LINE=${1:-32}
CACHE=${2:-1024}

declare -a STRATEGIES=('compressed' 'compressed' 'slice' 'slice' 'slice' 'ekmr')
declare -a ORIENTATIONS=('row' 'tube' 'lateral' 'horizontal' 'frontal' 'row')

for X in 10 50 100 150 300 400; do 
    printf "Processing: $X: "
    #for STRATEGY in ${STRATEGIES[@]}; do
    for i in {0..5}; do
	printf " ${STRATEGY} "
	for Y in {1..9}; do
	    FILE="tensor.100.$Y.tsr.${STRATEGIES[$i]}.${ORIENTATIONS[$i]}"
	    TENSOR="${FILE}.in"
	    RESULTS="${FILE}.results"
	    ./tensor effectuate -l ${LINE} -m ${CACHE} -o n-mode -s input/vector$X.in intermediate/${TENSOR} >results/${RESULTS} 2>&1
	    printf "."
	done
    done
    printf " [done].\n"
done
