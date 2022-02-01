#define _GNU_SOURCE

#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/resource.h>
#include "cacheutils.h"

int main (int ac, char **av) {
	// Assign the current process to only run on CPU 0
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(0, &mask);
	sched_setaffinity(0, sizeof(mask), &mask);

	// Assign niceness to low
	//nice(19);

	// Assign priority to low
	//setpriority(PRIO_PROCESS, getpid(), 19);


        long maxRange = 32768;
        char* array = (char *)malloc(maxRange + getpagesize());

	// Access array so block is loaded into the cache
	maccess(array);
	
	int trials = 0;
	size_t elapsedMissTime = 0;
	size_t elapsedHitTime = 0;

	for (int i = 0; i < 1000; i++) {
		// Time a cache hit
		size_t temp = rdtsc();
		maccess(array);
		size_t hitTime = rdtsc() - temp;

		// Flush the cache block
		flush(array);

		// Time a cache miss
		temp = rdtsc();
		maccess(array);
		size_t missTime = rdtsc() - temp;

		printf("Hit time: %lu\nMiss time: %lu\n", hitTime, missTime);

		elapsedHitTime += hitTime;
		elapsedMissTime += missTime;
		trials++;
	}

	printf("Average hit time: %lu\nAverage miss time: %lu\n", elapsedHitTime / (long)trials, elapsedMissTime / (long)trials);
}
