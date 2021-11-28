typedef struct reg
{
  //probably need to include load and enable bits
  int size; //number of bits in register
  char *vals; //perhaps we need to use unsigned char
} reg;

reg* init_register(int);

void reg_assign(reg*, char*);

int reg_access(reg*, int);

void print_register(reg*);

void del_register(reg *);
