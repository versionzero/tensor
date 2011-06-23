#!/bin/sh

if [ $# -ne 1 ]; then
    echo "usage: $0 N"
    exit 1
fi

N=$1

echo "%%MatrixMarket vector array real    general            "
echo $N

for i in `seq 1 $N`; do
    echo $i
done
