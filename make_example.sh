#!/bin/bash

declare -a STRATEGIES=('compressed' 'compressed' 'slice' 'slice' 'slice' 'ekmr')
declare -a ORIENTATIONS=('row' 'tube' 'lateral' 'horizontal' 'frontal' 'row')

NAME=${1:-example4}

for i in {0..4}; do
    STRATEGY=${STRATEGIES[$i]}
    ORIENTATION=${ORIENTATIONS[$i]}
    printf "$STRATEGY-$ORIENTATION:\n"
    ./tensor convert -s ${STRATEGY} -o ${ORIENTATION} -l ${NAME}.in #${NAME}.${STRATEGY}.${ORIENTATION}.in
done
