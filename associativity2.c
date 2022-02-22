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
#include <time.h>
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

    	int cachesize = 32000; //in b

	// Fill the pointer-chasing array
	int* indexes = (int*)malloc(N*(N+1)/2*sizeof(int));
	for (int i = 0; i < N*(N+1)/2; i++) {
		indexes[i] = i * cachesize;
	}

	// Fill the pointer-pointer-chasing array
	int** indexesToIndexes = (int**)malloc(N*(N+1)/2*sizeof(int*));
	for (int i = 0; i < N*(N+1)/2; i++) {
		indexesToIndexes[i] = indexes + i;
	}
	// Flush all of the array from the cache
	for (int i = 0; i < N*(N+1)/2; i++) {
		flush(array+i*cachesize);
	}

	int unusedIndex = 0;
        for (int blocks = 1 ; blocks <= N; blocks++){
		int startingIndex = unusedIndex;
		// Measure miss times of new blocks
		for (int i = startingIndex; i < startingIndex + blocks; i++) {
			size_t time1 = rdtsc();
			maccess(array+**(indexesToIndexes + i));
        		size_t delta1 = rdtsc() - time1;
			unusedIndex++;
			printf("Miss time of %ith of %i blocks: %i \n", i - startingIndex + 1, blocks, delta1);
		}
	        
		// Measure hit time of these new blocks. If these times begin to look like miss times,
		// we know the cache set has been filled
		for (int i = startingIndex; i < startingIndex + blocks; i++) {
        		size_t time2 = rdtsc();
			maccess(array+**(indexesToIndexes + i));
        		size_t delta2 = rdtsc() - time2;
			printf("Hit time of %ith of %i blocks: %i \n", i - startingIndex + 1, blocks, delta2); 
		}
		printf("\n");

		// Flush these blocks out of the cache
		for (int i = startingIndex; i < startingIndex + blocks; i++) {
			flush(array+**(indexesToIndexes + i));
		}
        }

	// Flush all of the array from the cache
	for (int i = 0; i < N*(N+1)/2; i++) {
		flush(array+i*cachesize);
	}
    
	free(array);
}
