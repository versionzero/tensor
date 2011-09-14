#!/bin/bash

VECTOR=${1:-vector3}
TENSOR=${2:-example7}
LINE=${3:-64}
CACHE=${4:-2048}

declare -a STRATEGIES=('compressed' 'compressed' 'slice' 'slice' 'slice' 'ekmr')
declare -a ORIENTATIONS=('row' 'tube' 'lateral' 'horizontal' 'frontal' 'row')

for i in {0..4}; do
    NAME=${TENSOR}.${STRATEGIES[$i]}.${ORIENTATIONS[$i]}
    #./tensor effectuate -s -l ${LINE} -m ${CACHE} -o n-mode -w ${VECTOR}.in ${NAME}.in ${NAME}.out \
    #	>${NAME}.results 2>${NAME}.error
    
    ./tensor effectuate -o n-mode -w ${VECTOR}.in ${NAME}.in ${NAME}.out \
	>${NAME}.results 2>${NAME}.error
done
