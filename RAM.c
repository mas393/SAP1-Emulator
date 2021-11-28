#include <stdio.h>
#include <stdlib.h>
#include "RAM.h"

RAM* init_RAM(int s, int b)
{
  RAM* mem;
  mem -> blocks = s;
  mem -> block_size = b; //in bits
  mem -> vals = malloc(s*(b/8 + 1));
  return mem;
}

void RAM_assign(RAM *mem, char *loc_nibble, char *data)
{
  int block = strtol(loc_nibble, 0, 2);
  char v = strtol(data, 0, 2);
  mem -> vals[block] = v;
}

char RAM_access(RAM *mem, char *loc_nibble)
{
  int block = strtol(loc_nibble, 0, 2);
  char val = mem -> vals[block];
  
  return val;
}

void print_RAM(RAM *mem)
{
  for (int i=0; i < mem -> blocks; i++) // i is accessing the characters which hold block_size bits each
    {
      for (int j=3; j >= 0; j--) printf("%d", (i >> j & 0x01));
      printf("  ");
      for (int j= mem -> block_size - 1; j >= 0; j--) // j is accessing the bit in range 0 to blocksize
	{
	  printf("%d", (mem->vals[i] >> j & 0x01));
	  if (j % 4 == 0) printf(" ");
	}
      printf("\n");
    }
}

void del_RAM(RAM *mem)
{
  free(mem -> vals);
  free(mem);
}
