for x in {0..2} ; do 
    valgrind --tool=cachegrind --cachegrind-out-file=cache.${x}.dat \
	--dsymutil=yes  ./tensor -s${x} -o2 input.dat \
	input.dat output.dat
done
