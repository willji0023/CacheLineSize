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

    	int cachesize = 32768; //in b
	
	// Fill the pointer-chasing array
	int* indexes = (int*)malloc(20*sizeof(int));
	for (int i = 0; i < 20; i++) {
		indexes[i] = i * cachesize;
	}

	// Fill the pointer-pointer-chasing array
	int** indexesToIndexes = (int**)malloc(20*sizeof(int*));
	for (int i = 0; i < 20; i++) {
		indexesToIndexes[i] = indexes + i;
	}

	// Flush all of the array from the cache
	for (int i = 0; i < 20; i++) {
		flush(array+**(indexesToIndexes + i));
	}

	// Time accesses until we get a miss
        for (int i = 0 ; i < 20; i++){
		maccess(array+**(indexesToIndexes+i)); // Access a new part of the array, add this to the cache

		for (int j = 0; j <= i; j++) {
        		size_t time = rdtsc();
			maccess(array+**(indexesToIndexes + j));
        		size_t delta = rdtsc() - time;
			printf("Timing of block %i: %i\n", j, delta); // TODO 
		}
		printf("\n");
        }

	// Flush all of the array from the cache
	for (int i = 0; i < 20; i++) {
		flush(array+**(indexesToIndexes + i));
	}
    
	free(array);
}
