

for p in fiber fiber-block slice slice-block; do 
    echo "Partitioning ${p}:"
    for x in {100..400..100}; do 
	echo "Tensor ${x}^3"
	for t in 1 2 4 8 16 32; do 
	    printf "${t}: "
	    ./tensor effectuate -o mode-1 -p ${p} -t ${t} ../data/array${x}.in ../data/vector${x}.in 2>&1 | \
		/bin/grep operation | cut -d[ -f2 | cut -d] -f1
	done
    done
done
