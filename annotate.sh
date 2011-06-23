#!/bin/sh -x

declare -a strategies=('compressed' 'ekmr' 'zzekmr')
for strategy in ${strategies[@]}; do
    cg_annotate --threshold=100 --auto=yes cache.${strategy}.dat >| annotate.${strategy}.h
done
