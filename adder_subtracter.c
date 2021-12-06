#include <stdio.h>
#include <stdlib.h>
#include "adder_subtracter.h"

adder_subtracter* init_adder_subtracter()
{
  adder_subtracter *temp;
  temp = malloc(sizeof(adder_subtracter));
  return temp;
}

char add_bits(reg *r1, reg *r2, int bit_loc, char *carry)
{
  char v = '0';

  switch (reg_access(r1, bit_loc) + reg_access(r2, bit_loc) + (int)*carry)
    {
    case 0:
      *carry = 0x0;
      break;
    case 1:
      *carry = 0x0;
      v = '1'; 
      break;
    case 2:
      *carry = 0x1;
      break;
    case 3:
      *carry = 0x1;
      v = '1';
      break;
    default:
      printf("here some how");
    }
 
  return v;
}

char* addition(adder_subtracter *as)
{
  //may have to pass in bus or provide it to adder_subtracter st temp is not lost
  char carry = 0x0;
  char *val = malloc(8);
  for (int i = 0; i < as -> BReg -> size; i++)
    {
      char v = add_bits(as -> Accumulator, as -> BReg, i, &carry);
      val[8-i-1] = v;
      
    }
  
  return val;
}

char* subtraction(adder_subtracter *as)
{
  char *val = malloc(8);
  int add_flag = 1;
  for (int i = 0; i < as -> BReg -> size; i++)
    {
      int loc = as -> BReg -> size - i - 1;
      if (add_flag)
	{
	  val[loc] = '0';
	  if (reg_access(as->BReg, i))
	    {
	      add_flag = 0;
	      val[loc] = '1';
	    }
	}
      else {
	if (reg_access(as->BReg, i)) val[loc] = '0';
	else val[loc] = '1';
      }
    }
  reg_assign(as -> BReg, val);
  return addition(as);
}
