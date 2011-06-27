#/bin/bash -x

if [ $# -le 3 ]; then
    echo "usage $0 <orientation> <size> <y> <iterations=10>"
    exit 1
fi

ORIENTATION=$1
SIZE=$2
Y=$3
ITERATIONS=${4:-10}

echo "${ORIENTATION}, ${SIZE}, $Y, ${ITERATIONS}"

SUFFIX="${ORIENTATION}-${SIZE}-$Y"
INPUT="${SUFFIX}.tsr"
RESULTS="${SUFFIX}.results"
GROUNDS="${SUFFIX}.grounds"

rm -f output/${SUFFIX}*

declare -a strategies=('compressed' 'ekmr')
for strategy in ${strategies[@]}; do
    # --I1=8192,1,64 --LL=1024,2,64
    valgrind --tool=cachegrind --dsymutil=yes --show-below-main=yes --cache-sim=yes --branch-sim=yes --dump-instr=yes --trace-jump=yes \
	--D1=2048,2,64 --LL=8192,2,64 --cachegrind-out-file=./output/${GROUNDS} ./tensor effectuate \
	-o n-mode -n ${ITERATIONS} input/vector${SIZE}.in input/${strategy}-${INPUT} 2>&1 | \
	ack -A 0 -B 0 --nocolor "(misses|refs)" >| output/${strategy}-${RESULTS}
done

#rm -f cache.*.dat
