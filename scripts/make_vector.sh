#!/bin/sh

if [ $# -ne 1 ]; then
    echo "usage: $0 N"
    exit 1
fi

N=$1

echo "%%MatrixMarket vector array real    general            "
echo $N

for (( i=1; i<=N; i++ )); do
    echo $i
done
