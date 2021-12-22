#ifndef RAM_H
#define RAM_H

#include "registers.h"

typedef struct RAM
{
  int blocks;
  int block_size;
  reg *cur_addr;
  char *vals;
} RAM;

RAM* init_RAM(int, int);

void set_RAM(RAM*, char*, char*, char*);

char* get_RAM(RAM*);

void print_RAM(RAM*);

void del_RAM(RAM*);

#endif
