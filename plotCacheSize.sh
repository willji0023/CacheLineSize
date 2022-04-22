printf "lines,access time\n" > cachesize.csv
for i in `seq 8 8 8192`
do
	for j in {1..20}
	do
		./cachesize $i 64 >> cachesize.csv
		printf "\r$i/8192 trials done..."
	done
done
printf "\n"
