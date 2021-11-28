typedef struct RAM
{
  int blocks;
  int block_size;
  char *vals;
} RAM;

RAM* init_RAM(int, int);

void RAM_assign(RAM*, char*, char*);

char RAM_access(RAM*, char*);

void print_RAM(RAM*);

void del_RAM(RAM*);
