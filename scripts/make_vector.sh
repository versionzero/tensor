#!/bin/sh

if [ $# -le 1 ]; then
    echo "usage: $0 N M"
    exit 1
fi

N=$1
M=${2:--1}

echo "%%MatrixMarket vector array real    general            "
echo $N

for (( i=1; i<=N; i++ )); do
    if [ $M = -1 ]; then
	echo $i
    else
	echo $M
    fi
done
