#include "registers.h"

typedef struct adder_subtracter
{
  reg *BReg;
  reg *Accumulator;
} adder_subtracter;

char add_bits(reg*, reg*, int, char*);

reg* addition(adder_subtracter*);

reg* subtraction(adder_subtracter*);
