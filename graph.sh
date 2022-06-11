#! /bin/bash
#16384
for j in `seq 8 1 9`
do
	printf "Array Size (B),Throughput\n" > stride${j}.csv
	for i in `seq 1 1 500`
	do
		./cachesizeTIME $j >> stride${j}.csv
		#printf "\n" >> stride${j}.csv
	done
done