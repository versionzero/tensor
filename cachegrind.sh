#/bin/sh -x

if [ $# -le 2 ]; then
    echo "usage $0 <orientation> <size> <iterations=10>"
    exit 1
fi

ORIENTATION=$1
SIZE=$2
ITERATIONS=${3:-10}
SUFFIX="${ORIENTATION}-tensor${SIZE}.out"

rm -f *${SUFFIX} cache.*.dat

declare -a strategies=('compressed' 'ekmr') # 'zzekmr')
for strategy in ${strategies[@]}; do
    # --I1=8192,1,64 --LL=1024,2,64
    valgrind --tool=cachegrind --dsymutil=yes --show-below-main=yes --cache-sim=yes --branch-sim=yes \
	--D1=2048,2,64 --LL=8192,2,64 --cachegrind-out-file=cache.${strategy}.dat ./tensor effectuate \
	-o n-mode -n ${ITERATIONS} vector${SIZE}.in samples/${strategy}-${SUFFIX} ${strategy}-${SUFFIX}
done
