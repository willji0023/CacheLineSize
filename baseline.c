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
#include "cacheutils.h"

int main (int ac, char **av) {		
	long maxRange = 32768;
	char* array = (char *)malloc(maxRange + getpagesize());
	
	// Access array so block is loaded into the cache
	maccess(array);

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
}
