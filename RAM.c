#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RAM.h"


RAM* init_RAM(int s, int b)
{
  RAM* mem = malloc(sizeof(RAM));
  mem -> blocks = s;
  mem -> block_size = b; //in bits
  mem -> vals = malloc(s*(b/8 + 1));
  return mem;
}

void set_RAM(RAM *mem, char *loc_nibble, char *inst, char *data)
{
  int block = strtol(loc_nibble, 0, 2);
  unsigned char v = strtol(data, 0, 2);
  unsigned char h = strtol(inst, 0, 2);
  h = h << 4;
  h += v;
  memcpy(mem -> vals + block, &h, 1);
}

char* get_RAM(RAM *mem)
{
  char *val = malloc(mem -> block_size);
  char *regcpy = get_reg(mem->cur_addr, 4, 0);
  int block = strtol(regcpy, 0, 2);
  free(regcpy);
  char c = mem -> vals[block];

  for (int j = 0; j < mem -> block_size; j++)
    {
      val[j] = ((c >> (mem -> block_size - 1 - j)) & 0x1) ? '1': '0';
    }
  
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
