#ident "$Id: rdmsr.c,v 1.4 2004/07/20 15:54:59 hpa Exp $"
/* ----------------------------------------------------------------------- *
 *   
 *   Copyright 2000 Transmeta Corporation - All Rights Reserved
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 675 Mass Ave, Cambridge MA 02139,
 *   USA; either version 2 of the License, or (at your option) any later
 *   version; incorporated herein by reference.
 *
 * ----------------------------------------------------------------------- */

/*
 * rdmsr.c
 *
 * Utility to read an MSR.
 */

#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <inttypes.h>
#include <sys/types.h>

#include "version.h"

struct option long_options[] = {
  { "help",                0, 0, 'h' },
  { "version",             0, 0, 'V' },
  { "hexadecimal",         0, 0, 'x' },
  { "capital-hexadecimal", 0, 0, 'X' },
  { "decimal",             0, 0, 'd' },
  { "signed-decimal",      0, 0, 'd' },
  { "unsigned-decimal",    0, 0, 'u' },
  { "octal",               0, 0, 'o' },
  { "c-language",          0, 0, 'c' }, 
  { "zero-fill",           0, 0, '0' },
  { "zero-pad",            0, 0, '0' },
  { "raw",                 0, 0, 'r' },
  { "processor",           1, 0, 'p' },
  { "cpu",                 1, 0, 'p' },
  { "bitfield",            1, 0, 'f' },
  { 0, 0, 0, 0 }
};

/* Number of decimal digits for a certain number of bits */
/* (int) ceil(log(2^n)/log(10)) */
int decdigits[] = {
  1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 4, 5, 5,
  5, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10,
  10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15,
  15, 15, 16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 19,
  20
};

#define mo_hex  0x01
#define mo_dec  0x02
#define mo_oct  0x03
#define mo_raw  0x04
#define mo_uns  0x05
#define mo_chx  0x06
#define mo_mask 0x0f
#define mo_fill 0x40
#define mo_c    0x80

const char *program;

void usage(void)
{
  fprintf(stderr,
	  "Usage: %s [options] regno\n"
	  "  --help         -h  Print this help\n"
	  "  --version      -V  Print current version\n"
	  "  --hexadecimal  -x  Hexadecimal output (lower case)\n"
	  "  --capital-hex  -X  Hexadecimal output (upper case)\n"
	  "  --decimal      -d  Signed decimal output\n"
	  "  --unsigned     -u  Unsigned decimal output\n"
	  "  --octal        -o  Octal output\n"
	  "  --c-language   -c  Format output as a C language constant\n"
	  "  --zero-pad     -0  Output leading zeroes\n"
	  "  --raw          -r  Raw binary output\n"
	  "  --processor #  -p  Select processor number (default 0)\n"
	  "  --bitfield h:l -f  Output bits [h:l] only\n"
	  , program);
}

int main(int argc, char *argv[])
{
  uint32_t reg;
  uint64_t data;
  int c, fd;
  int mode = mo_hex;
  int cpu = 0;
  unsigned int highbit = 63, lowbit = 0, bits;
  unsigned long arg;
  char *endarg;
  char *pat;
  int width;
  char msr_file_name[64];

  program = argv[0];

  while ( (c = getopt_long(argc,argv,"hVxXdoruc0p:f:",long_options,NULL)) != -1 ) {
    switch ( c ) {
    case 'h':
      usage();
      exit(0);
    case 'V':
      fprintf(stderr, "%s: version %s\n", program, VERSION_STRING);
      exit(0);
    case 'x':
      mode = (mode & ~mo_mask) | mo_hex;
      break;
    case 'X':
      mode = (mode & ~mo_mask) | mo_chx;
      break;
    case 'o':
      mode = (mode & ~mo_mask) | mo_oct;
      break;
    case 'd':
      mode = (mode & ~mo_mask) | mo_dec;
      break;
    case 'r':
      mode = (mode & ~mo_mask) | mo_raw;
      break;
    case 'u':
      mode = (mode & ~mo_mask) | mo_uns;
      break;
    case 'c':
      mode |= mo_c;
      break;
    case '0':
      mode |= mo_fill;
      break;
    case 'p':
      arg = strtoul(optarg, &endarg, 0);
      if ( *endarg || arg > 255 ) {
	usage();
	exit(127);
      }
      cpu = (int)arg;
      break;
    case 'f':
      {
	if ( sscanf(optarg, "%u:%u", &highbit, &lowbit) != 2 ||
	     highbit > 63 || lowbit > highbit ) {
	  usage();
	  exit(127);
	}
      }
      break;
    default:
      usage();
      exit(127);
    }
  }

  if ( optind != argc-1 ) {
    /* Should have exactly one argument */
    usage();
    exit(127);
  }

  reg = strtoul(argv[optind], NULL, 0);

  sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
  fd = open(msr_file_name, O_RDONLY);
  if ( fd < 0 ) {
    if ( errno == ENXIO ) {
      fprintf(stderr, "rdmsr: No CPU %d\n", cpu);
      exit(2);
    } else if ( errno == EIO ) {
      fprintf(stderr, "rdmsr: CPU %d doesn't support MSRs\n", cpu);
      exit(3);
    } else {
      perror("rdmsr:open");
      exit(127);
    }
  }
  
  if ( pread(fd, &data, sizeof data, reg) != sizeof data ) {
    perror("rdmsr:pread");
    exit(127);
  }

  close(fd);

  bits = highbit-lowbit+1;
  if ( bits < 64 ) {
    /* Show only part of register */
    data >>= lowbit;
    data &= (1ULL << bits)-1;
  }

  pat = NULL;

  width = 1;			/* Default */
  switch(mode) {
  case mo_hex:
    pat = "%*llx\n";
    break;
  case mo_chx:
    pat = "%*llX\n";
    break;
  case mo_dec:
  case mo_dec|mo_c:
  case mo_dec|mo_fill|mo_c:
    /* Make sure we get sign correct */
    if ( data & (1ULL << (bits-1)) ) {
      data &= ~(1ULL << (bits-1));
      data = -data;
    }
    pat = "%*lld\n";
    break;
  case mo_uns:
    pat = "%*llu\n";
    break;
  case mo_oct:
    pat = "%*llo\n";
    break;
  case mo_hex|mo_c:
    pat = "0x%*llx\n";
    break;
  case mo_chx|mo_c:
    pat = "0x%*llX\n";
    break;
  case mo_oct|mo_c:
    pat = "0%*llo\n";
    break;
  case mo_uns|mo_c:
  case mo_uns|mo_fill|mo_c:
    pat = "%*lluU\n";
    break;
  case mo_hex|mo_fill:
    pat = "%0*llx\n";
    width = (bits+3)/4;
    break;
  case mo_chx|mo_fill:
    pat = "%0*llX\n";
    width = (bits+3)/4;
    break;
  case mo_dec|mo_fill:
    /* Make sure we get sign correct */
    if ( data & (1ULL << (bits-1)) ) {
      data &= ~(1ULL << (bits-1));
      data = -data;
    }
    pat = "%0*lld\n";
    width = decdigits[bits-1]+1;
    break;
  case mo_uns|mo_fill:
    pat = "%0*llu\n";
    width = decdigits[bits];
    break;
  case mo_oct|mo_fill:
    pat = "%0*llo\n";
    width = (bits+2)/3;
    break;
  case mo_hex|mo_fill|mo_c:
    pat = "0x%0*llx\n";
    width = (bits+3)/4;
    break;
  case mo_chx|mo_fill|mo_c:
    pat = "0x%0*llX\n";
    width = (bits+3)/4;
    break;
  case mo_oct|mo_fill|mo_c:
    pat = "0%0*llo\n";
    width = (bits+2)/3;
    break;
  case mo_raw:
  case mo_raw|mo_fill:
    fwrite(&data,sizeof data,1,stdout);
    break;
  case mo_raw|mo_c:
  case mo_raw|mo_fill|mo_c:
    {
      unsigned char *p = (unsigned char *)&data;
      int i;
      for ( i = 0 ; i < sizeof data ; i++ ) {
	printf("%s0x%02x", i?",":"{", (unsigned int)(*p++));
      }
      printf("}\n");
    }
  break;
  default:
    fprintf(stderr, "%s: Impossible case, line %d\n", program, __LINE__);
    exit(127);
  }

  if ( width < 1 )
    width = 1;

  if ( pat )
    printf(pat, width, data);

  exit(0);
}
