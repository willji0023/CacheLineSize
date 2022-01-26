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
	long maxStride = 2048;
	long maxRange = 1048576;
	FILE *file =  fopen("results.csv", "w+");
	fputs("stride,iteration,latency\n", file);
	char* array = (char *)malloc(maxRange + getpagesize());

	for (long stride = 1; stride <= maxStride; stride*=2) {
		for(int i = 0; i < 100; i++){
			flush(array + i*stride);
		}
		size_t time = rdtsc();
		for(int i = 0; i < 100; i++){
			maccess(array + i*stride);
		}
		size_t delta = rdtsc() - time;
		fprintf(file, "%ld,%lu\n", stride, delta);
		printf("%ld,%lu\n",  stride, delta);
	}
}
