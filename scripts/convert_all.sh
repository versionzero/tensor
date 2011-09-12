#!/bin/bash

FILE=${1:-example7}

declare -a STRATEGIES=('compressed' 'compressed' 'slice' 'slice' 'slice' 'ekmr')
declare -a ORIENTATIONS=('row' 'tube' 'lateral' 'horizontal' 'frontal' 'row')

for i in {0..4}; do
    NAME=${FILE}.${STRATEGIES[$i]}.${ORIENTATIONS[$i]}
    ./tensor convert -s ${STRATEGIES[$i]} -o ${ORIENTATIONS[$i]} ${FILE}.in ${NAME}.in
done
