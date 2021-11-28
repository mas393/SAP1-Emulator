typedef struct reg
{
  //probably need to include load and enable bits
  int size; //number of bits in register
  char *vals; //perhaps we need to use unsigned char
} reg;

reg* init_register(int);

void reg_assign(reg*, char*);

int reg_access(reg*, int);

char* get_reg(reg*, int s);

void print_register(reg*);

void del_register(reg *);

typedef struct RAM
{
  int blocks;
  int block_size;
  reg *cur_addr;
  char *vals;
} RAM;

RAM* init_RAM(int, int);

void RAM_assign(RAM*, char*, char*);

char* get_RAM(RAM*);

void print_RAM(RAM*);

void del_RAM(RAM*);
