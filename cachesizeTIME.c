#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sched.h>
#include <sys/resource.h>

#define MAXBYTES (1 << 27)
#define MAXELEMS MAXBYTES/sizeof(long) 
long array[MAXELEMS];

u_int64_t rdtsc() {
  u_int64_t a, d;
  asm volatile ("mfence");
  asm volatile ("rdtsc" : "=a" (a), "=d" (d));
  a = (d<<32) | a;
  asm volatile ("mfence");
  return a;
}

long keepDummy(long dummy){
    return dummy;
}

long run(int num, int STRIDE){
    long i;
    long sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;

    //array - An array of longs which is of size 128MB
    //num - The number of array elements we want to access
    //      aka the buffer size
    //STRIDE - The access pattern ex. A stride of 8 would
    //         access every 8th element of the buffer
    for(i = 0; i < num-STRIDE*4; i += STRIDE*4){
		sum0 = sum0 + array[i];
        sum1 = sum1 + array[i+STRIDE];
        sum2 = sum2 + array[i+STRIDE*2];
        sum3 = sum3 + array[i+STRIDE*3];
	}
    for(;i < num; i++){
        sum0 = sum0 + array[i];
    }
    return ((sum0+sum1)+(sum2+sum3));
}
// long run(int num, int STRIDE){
//     long i;
//     long sum0 = 0;

//     for(i = 0; i < num-STRIDE; i += STRIDE){
// 		sum0 = sum0 + array[i];
// 	}
//     return (sum0);
// }

int main (int argc, char **argv) {
    cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(0, &mask);
	sched_setaffinity(0, sizeof(mask), &mask);
    setpriority(PRIO_PROCESS, getpid(), 0);
    long dummy = 0;
	int i;
    int j;
	long int sum = 0;
    int STRIDE = atoi(argv[1]);
    for(i = 0; i < MAXELEMS; i++){
        array[i] = i;
    }

    size_t delta = 0;
    size_t t;
    double time;
    for(unsigned long long i = 1 << 27; i >= 4096; i = i/2){
        delta = 0;
        for(int k = 0; k < 5; k++){
            dummy += run(i/sizeof(long), STRIDE);
            dummy = 0;
        }
        t = rdtsc();
        for(int j = 0; j < 100; j++){
	        dummy +=run(i/sizeof(long), STRIDE);
            dummy = 0;
        }
        delta = rdtsc() - t;
        time = delta/2200.0;
        printf("%lld, %.0f\n", i/1024, (i*100)/time);
    }

	return 0;
}
