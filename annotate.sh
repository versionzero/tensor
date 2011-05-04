#!/bin/sh
for x in {0..2} ; do 
    cg_annotate cache.${x}.dat --threshold=100 --auto=yes >| annotate.${x}.h
done
