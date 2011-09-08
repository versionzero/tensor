#!/bin/bash

VECTOR=${1:-vector4}
TENSOR=${2:-example4}
LINE=${3:-64}
CACHE=${4:-2048}

declare -a STRATEGIES=('compressed' 'compressed' 'slice' 'slice' 'slice' 'ekmr')
declare -a ORIENTATIONS=('row' 'tube' 'lateral' 'horizontal' 'frontal' 'row')

for i in {0..5}; do
    NAME=${TENSOR}.${STRATEGIES[$i]}.${ORIENTATIONS[$i]}
    ./tensor effectuate -l ${LINE} -m ${CACHE} -o n-mode -s ${VECTOR}.in ${NAME}.in ${NAME}.out \
	>${NAME}.results 2>${NAME}.error
done
