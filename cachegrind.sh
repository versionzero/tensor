#/bin/sh

rm -f *-row-tensor100.out

declare -a strategies=('compressed' 'ekmr' 'zzekmr')
for strategy in ${strategies[@]}; do
    valgrind --tool=cachegrind --cachegrind-out-file=cache.${strategy}.dat \
	--dsymutil=yes  ./effectuate -o n-mode vector100.in \
	tests/${strategy}-row-tensor100.out ${strategy}-row-tensor100.out
done
