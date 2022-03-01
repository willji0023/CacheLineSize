#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main (int ac, char **av) {
	int CACHE_IDS = 16;

	int last_level = 0;
	for (int i  = 0; i < CACHE_IDS; i++) {
        	/** 
		 * Variables to hold the contents of the 4 i386 
		 * legacy registers
		 */
        	uint32_t eax, ebx, ecx, edx; 
		eax = 4; // Initial value to get cache parameters from CPUID
		ecx = i; // ID of cache (usually starting at 0)

		__asm__ (
		  "cpuid" // The i386 cpuid instruction
		  : "+a" (eax) // Input containing the command code
		  , "=b" (ebx)
		  , "+c" (ecx) // Input containing the cache ID
		  , "=d" (edx)
		); // Generates output in the 4 registers above

		/**
		 * According to Intel Manual, if ECX contains an invalid
		 * ID, EAX/EBX/ECX/EDX return 0.
		 */
		if (!eax && !ebx && !ecx && !edx) {
			// printf("%i is not a valid cache ID!\n", i);
			exit(0);
		} else {
			/** 
			 * According to Intel Manual, EAX Bits 07-05 contain
			 * cache level
		         * 111 (binary) = 7 (hexadecimal)
			 */
			int level = (eax >>= 5) & 0x7;
			if (level <= last_level) continue;
			last_level++;
			// printf("Using cache ID %i:\n", i);

			/**
			 * According to Intel Manual, EBX Bits 11-00 contain
			 * system coherency line size- 1 and EBX Bits 21-12 
			 * contain physical line partitions - 1
			 * 1111 1111 1111 (binary) = 7 (hexadecimal)
			 */
			int coherency_line_size = (ebx & 0xFFF) + 1;
			int line_partitions = ((ebx >> 12) & 0x3FF) + 1;
			int cache_line_size
				= coherency_line_size * line_partitions;
			printf("L%i Cache Line Size: %i\n", 
					level, cache_line_size);

			/**
			 * According to Intel Manual, ECX Bits 31-00 contain
			 * the number of sets - 1
			 */
			int num_sets = ecx + 1;
			int total_size = cache_line_size * num_sets;
			printf("L%i Cache Size: %i\n", level, total_size);

			/**
			 * According to Intel Manual, EBX Bits 31-22 contain
			 * the ways of associativity - 1
			 * 11 1111 1111 (binary) = 3FF (hexadecimal)
			 */
			int associativity = ((ebx >> 22) & 0x3FF) + 1;
			printf("L%i Associativity: %i\n", level, associativity);
		}		
	}
}
