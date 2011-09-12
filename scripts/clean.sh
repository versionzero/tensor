#!/bin/bash

DIR=${1:-`dirname $0`}

declare -a STRATEGIES=('compressed' 'compressed' 'slice' 'slice' 'slice' 'ekmr')
declare -a ORIENTATIONS=('row' 'tube' 'lateral' 'horizontal' 'frontal' 'row')

for i in {0..5}; do
    NAME="${DIR}/*.${STRATEGIES[$i]}.${ORIENTATIONS[$i]}"
    rm -f ${NAME}.in ${NAME}.out ${NAME}.results ${NAME}.error
done
