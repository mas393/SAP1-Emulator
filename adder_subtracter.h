#ifndef ADDER_SUBTRACTER_H
#define ADDER_SUBTRACTER_H

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

#endif
