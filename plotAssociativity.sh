for (( cachesize = 256; cachesize <= 32768; cachesize*=2 )); do
	printf "number of accesses (i * cachesize),access time\n" > associativity${cachesize}.csv
	echo "Testing cachesize = $cachesize: "
	for i in `seq 1 1 18`
	do
		for j in {1..100}
		do
			./associativity3 $cachesize $i >> associativity${cachesize}.csv

			printf "\r$i/18 trials done..."
		done
	done
	printf "\n"
done
