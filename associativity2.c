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
	// Unoptimize program
    	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(0, &mask);
	sched_setaffinity(0, sizeof(mask), &mask);
	setpriority(PRIO_PROCESS, getpid(), 0);
	nice(19);

	// Create array and parameters for benchmark
	int N = atoi(av[2]);
    	int cachesize = atoi(av[1]); //in b
	long maxRange = 32000000;
	char* array = (char*)malloc(maxRange); //[0] [cachesize*1] [cachesize*2][cachesize*3]

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
		flush(array+**(indexesToIndexes + i));
	}

	int hitTimes[N*(N+1)/2];
	int missTimes[N*(N+1)/2];

	int unusedIndex = 0;
        for (int blocks = 1 ; blocks <= N; blocks++){
		int startingIndex = unusedIndex;
		// Measure miss times of new blocks
		for (int i = startingIndex; i < startingIndex + blocks; i++) {
			int missTimeSum = 0;
			// Measure it 100 times and take the average
			for (int j = 0; j < 100; j++) {
				flush(array+**(indexesToIndexes + i));
				size_t time = rdtsc();
				maccess(array+**(indexesToIndexes + i));
        			size_t delta = rdtsc() - time;
				missTimeSum += delta;
			}
			missTimes[i] = missTimeSum / 100;
			unusedIndex++;
		}
	        
		// Measure hit time of these new blocks.
		// If these times begin to look like miss times,
		// we know the cache set has been filled
		for (int i = startingIndex; i < startingIndex + blocks; i++) {
			int hitTimeSum = 0;
			// Measure it 100 times and take the average
			for (int j = 0; j < 100; j++) {
	        		size_t time = rdtsc();
				maccess(array+**(indexesToIndexes + i));
        			size_t delta = rdtsc() - time;
				hitTimeSum += delta;
			}
			hitTimes[i] = hitTimeSum / 100;
		}

		// Flush these blocks out of the cache
		for (int i = startingIndex; i < startingIndex + blocks; i++) {
			flush(array+**(indexesToIndexes + i));
		}
        }

	// Flush all of the array from the cache
	for (int i = 0; i < N*(N+1)/2; i++) {
		flush(array+**(indexesToIndexes + i));
	}

	// Print out the miss and hit times
	int index = 0;
	for (int blocks = 1; blocks <= N; blocks++) {
		int lastIndex = index;
		for (int j = 1; j <= blocks; j++) {
			printf("Miss time of %ith of %i blocks: %i \n", j, blocks, missTimes[index]);
			index++;	
		}
		index = lastIndex;
		for (int j = 1; j <= blocks; j++) {
			printf("Hit time of %ith of %i blocks: %i \n", j, blocks, hitTimes[index]);
			index++;
		}
		printf("\n");
	}

	free(array);
}
