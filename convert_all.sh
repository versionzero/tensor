#!/bin/bash

declare -a STRATEGIES=('compressed' 'compressed' 'slice' 'slice' 'slice')
declare -a ORIENTATIONS=('row' 'tube' 'lateral' 'horizontal' 'frontal')

for i in {0..4}; do
    ./tensor convert -s ${STRATEGIES[$i]} -o ${ORIENTATIONS[$i]} ieee-fig4.in ieee-fig4.${ORIENTATIONS[$i]}.in
done
