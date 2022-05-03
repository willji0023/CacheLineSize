/**
 * Cache associtativity microbenchmark
 *
 * Example usage: ./associativity3 32768 4
 * (assuming a cache size of 32 kilobytes)
 */

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

/* Arrange the N elements of ARRAY in random order.
   Only effective if N is much smaller than RAND_MAX;
   if this may not be the case, use a better random
   number generator. */
void shuffleIndices(int *array, size_t n)
{
    if (n > 1) 
    {
        size_t i;
        for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

void shuffleAddresses(char **array, size_t n)
{
    if (n > 1) 
    {
        size_t i;
        for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          char* t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}


int main (int ac, char **av) {
	// Unoptimize program
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(0, &mask);
	sched_setaffinity(0, sizeof(mask), &mask);
	setpriority(PRIO_PROCESS, getpid(), 0);
	nice(19);

	// Create array and parameters for benchmark
	long maxRange = 32000000;
	char* array = (char*)aligned_alloc(1024, maxRange);
	int cachesize = atoi(av[1]);
	int num_access = atoi(av[2]);

	// Initalize a 2-element structure to access the array in a
	// randomized order
	char** addresses = (char**)malloc(sizeof(char*)*num_access);
	int* indices = (int*)malloc(sizeof(int)*num_access);
	for (int i = 0; i < num_access; i++){
		addresses[i] = array + i*cachesize;
		indices[i] = i;
	}
	shuffleAddresses(addresses, num_access);
	shuffleIndices(indices, num_access);

	// Access loop
	for (int i = 0 ; i < num_access ; i++){
		maccess(array + i*cachesize);
	}

	// Timing loop
	size_t delta = 0;
	for(int i = 0; i < num_access; i++){
		size_t t = rdtsc();
		maccess(addresses[indices[i]]);
		delta += rdtsc() - t;
	}

	// Flush the array from the cache after timing loop ends
	for (int i = 0 ; i < num_access ; i++){
		flush(array+i*cachesize);
	}

	// Print the average access time
	printf("%i, %f\n", num_access, (float) (delta/num_access));
	free(indices);
	free(addresses);
	free(array);
}
