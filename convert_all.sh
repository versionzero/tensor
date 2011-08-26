#!/bin/bash

declare -a STRATEGIES=('compressed' 'compressed' 'slice' 'slice' 'slice' 'ekmr')
declare -a ORIENTATIONS=('row' 'tube' 'lateral' 'horizontal' 'frontal' 'row')

for k in {1..9}; do
    FILE=input/tensor.100.$k.tsr
    for i in {0..5}; do
	./tensor convert -s ${STRATEGIES[$i]} -o ${ORIENTATIONS[$i]} ${FILE} ${FILE}.${STRATEGIES[$i]}.${ORIENTATIONS[$i]}.in
    done
done
