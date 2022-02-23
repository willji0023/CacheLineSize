CC = gcc
CLANG++ = clang
OPTS = -O3 -std=gnu99 -D_DEFAULT_SOURCE
FUZZ_OPTIONS = -O0 -std=c99 -D_DEFAULT_SOURCE -fsanitize=fuzzer-no-link,address,signed-integer-overflow -fsanitize-coverage=trace-cmp
LIBS = -lm

build:
	$(CC) $(OPTS) cachelinesize.c -o cachelinesize $(LIBS)
	$(CC) $(OPTS) baseline.c -o baseline $(LIBS)
	$(CC) $(OPTS) cachesize.c -o cachesize $(LIBS)
	$(CC) $(OPTS) associativity.c -o associativity $(LIBS)
	$(CC) $(OPTS) detection.c -o detection $(LIBS)

build-fuzzer:
	$(CLANG++) $(FUZZ_OPTIONS) cachelinesize.c -o cachelinesize $(LIBS)

clean:
	rm cachelinesize
	rm baseline
	rm associativity
	rm cachesize
