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

#include <time.h>

/*
 * Just reads a memory position. As you can see, the use_result_dummy() isn't declared
 * inside the function, being global instead. This is because this function will the called
 * several times and the overhead to alocate a long variable would impact in the results
 * of the program.
 */
long use_result_dummy;
void use_pointer( void *result ) {
	use_result_dummy += (long)result;
}

unsigned long now() {
	struct timespec tp;
	clock_gettime( CLOCK_MONOTONIC, &tp );
	return (unsigned long) (tp.tv_sec * 1000000000) + (long) (tp.tv_nsec) ;
}

int main (int ac, char **av) {		
	long maxStride = 2048;
	long maxRange = 1048576;
	int numTries = 3;
	FILE *file =  fopen("results.csv", "w+");
	fputs("range,stride,latency\n", file);
	char* array = (char *)malloc(maxRange + getpagesize());

	for (long stride = 1; stride <= maxStride; stride*=2) {
		for (long range = 1; range <= maxRange; range+=10000) {
			// Pointer Chasing
			register char **p = 0;	// p contains a memory address
			long best = 2000000000, // this is an upper limit for best
			
			/* makes i become the largest value possible within the range given the stride */
			i = stride * (range / stride);
	
			/* p will receive the address of the ??*/
			for (; i >= 0; i -= stride) {
				char	*next;

				p = (char **)&array[i];
				if (i < stride) {
					next = &array[range - stride];
				} else {
					next = &array[i - stride];
				}
				*p = next;
			}

			// Get the fastest access time of 3 tries
			for (int tries = 0; tries < numTries; ++tries) {
				unsigned long time = now();
				use_pointer((void *)p);
				time = now() - time;
				if (time < best) {
					best = time;
				}
			}
			fprintf(file, "%ld,%ld,%lu\n", range, stride, best);
			printf("%ld,%ld,%lu\n", range, stride, best);
		}
	}
}

