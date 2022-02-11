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

    	int cachesize = 64000; //in b

	int i = 128;
        for (int j = 0 ; j < i ; j++){
            maccess(array+j*cachesize);
        }
        size_t time = rdtsc();
        for (int j = 0 ; j < i ; j++){
        	size_t time = rdtsc();
		maccess(array+j*cachesize);
        	size_t delta = rdtsc() - time;
		printf("Accessing array at index %i (block %i): %i\n", j*cachesize, j, delta);
        }
	for(int j = 0; j < i; j++){
		flush(array + (j*cachesize));
	}
    
	free(array);
}
