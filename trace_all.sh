#!/bin/bash

declare -a STRATEGIES=('compressed' 'compressed' 'slice' 'slice' 'slice')
declare -a ORIENTATIONS=('row' 'tube' 'lateral' 'horizontal' 'frontal')

for i in {0..4}; do
    ./tensor effectuate -l 8 -m 32 -o n-mode -s input/vector2.in ieee-fig4.${ORIENTATIONS[$i]}.in ieee-fig4.${ORIENTATIONS[$i]}.out
done
