#include "registers.h"

typedef struct adder_subtracter
{
  reg *Accumulator;
  reg *BReg;
} adder_subtracter;

adder_subtracter* init_adder_subtracter();

char add_bits(reg*, reg*, int, char*);

char* addition(adder_subtracter*);

char* subtraction(adder_subtracter*);
