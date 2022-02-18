#define _GNU_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/resource.h>
#include "cacheutils.h"

int main (int ac, char **av) {
	int N = 20;

    	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(0, &mask);
	sched_setaffinity(0, sizeof(mask), &mask);
	setpriority(PRIO_PROCESS, getpid(), 0);
	nice(19);
    	int times[2048] = {0};
	long maxStride = 2048;
	long maxRange = 32000000;
	FILE *file =  fopen("results.csv", "w+");
	fputs("iteration,latency\n", file);
	char* array = (char*)malloc(maxRange); //[0] [cachesize*1] [cachesize*2][cachesize*3]

    	int cachesize = 4096; //in b

	// Flush all of the array from the cache
	for (int i = 0; i < N*(N+1)/2; i++) {
		flush(array+i*cachesize);
	}

	int unusedIndex = 0;
        for (int blocks = 1 ; blocks <= N; blocks++){
		int startingIndex = unusedIndex;
		for (int i = startingIndex, count = 1; count <= blocks; count++) {
			size_t time1 = rdtsc();
			maccess(array+i*cachesize);
        		size_t delta1 = rdtsc() - time1;
			unusedIndex++;
			printf("Miss time of %ith of %i blocks: %i \n", count, blocks, delta1);
			i++;
		}
	       	       
		for (int i = startingIndex, count = 1; count <= blocks; count++){
        		size_t time2 = rdtsc();
			maccess(array+i*cachesize);
        		size_t delta2 = rdtsc() - time2;
			printf("Hit time of %ith of %i blocks: %i \n", count, blocks, delta2); 
			i++;
		}
		printf("\n");

		for (int i = startingIndex, count = 1; count <= blocks; count++){
			flush(array+i*cachesize);
			i++;
		}
        }

	// Flush all of the array from the cache
	for (int i = 0; i < N*(N+1)/2; i++) {
		flush(array+i*cachesize);
	}
    
	free(array);
}
