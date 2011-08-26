#!/bin/bash

declare -a STRATEGIES=('compressed' 'compressed' 'slice' 'slice' 'slice' 'ekmr')
declare -a ORIENTATIONS=('row' 'tube' 'lateral' 'horizontal' 'frontal' 'row')

for X in 150 200 250 300 350 400; do
    for Y in {1..5}; do
	FILE=input/tensor.$X.$Y.tsr
	for i in {0..4}; do
	    ./tensor convert -s ${STRATEGIES[$i]} -o ${ORIENTATIONS[$i]} ${FILE} ${FILE}.${STRATEGIES[$i]}.${ORIENTATIONS[$i]}.in
	done
    done
done
