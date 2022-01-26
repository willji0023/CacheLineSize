CC = gcc
CLANG++ = clang
OPTS = -O3 -std=gnu99 -D_DEFAULT_SOURCE
FUZZ_OPTIONS = -O0 -std=c99 -D_DEFAULT_SOURCE -fsanitize=fuzzer-no-link,address,signed-integer-overflow -fsanitize-coverage=trace-cmp
LIBS = -lm

build:
	$(CC) $(OPTS) cachelinesize.c -o cachelinesize $(LIBS)

build-fuzzer:
	$(CLANG++) $(FUZZ_OPTIONS) cachelinesize.c -o cachelinesize $(LIBS)

clean:
	rm cachelinesize	
