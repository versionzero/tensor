#!/bin/bash

CACHE=${1:-2048}
ORIENTATION="row"
declare -a STRATEGIES=('compressed' 'ekmr')

for X in 10 50 100 150 300 400; do 
    printf "Processing: $X: "
    for STRATEGY in ${STRATEGIES[@]}; do
	printf " ${STRATEGY} "
	for Y in {1..10}; do
	    FILE="${STRATEGY}-${ORIENTATION}-$X-$Y"
	    TENSOR="${FILE}.tsr"
	    RESULTS="${FILE}.results"
	    ./tensor effectuate -m ${CACHE} -o n-mode -s input/vector$X.in input/${TENSOR} >results/${RESULTS} 2>&1
	    printf "."
	done
    done
    printf " [done].\n"
done
