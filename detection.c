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
    long H = 16; 
    int S = 0;
    long maxRange = 33554432;
    int MaxAssoc = 16;
    long maxStride = 2048;
    char* array = (char*)malloc(maxRange);
    size_t prev_time = 0;
    size_t curr_time;
    size_t time;
    FILE *file =  fopen("detectionres.csv", "w+");
    while((H * MaxAssoc) < maxRange){
        while (S < MaxAssoc){
            time = rdtsc();
            maccess(array + (S*H));
            curr_time = rdtsc() - time;
            fprintf(file, "%d,%d, %ld\n", S, H, curr_time);
            S = S + 1;
            prev_time = curr_time;
        }
        for(long i = 0; i < MaxAssoc; i++){
            flush(array + (i*H));
        }
        S = 1;
        H *= 2;
    }
}
