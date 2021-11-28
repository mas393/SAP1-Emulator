#include <stdio.h>
#include <stdlib.h>
#include "registers.h"
#include <string.h>

reg* init_register(int s) // initializing and filling of register are seperate items b/c in reality the register is initialized when it is made
{
  reg *r = malloc(sizeof(reg));
  r -> size = s;
  r -> vals = malloc(s/8+1);
  return r;
}

//would we assign bit by bit or the entire register? I am leaning towards the entire register b/c the bus carries all 8 bits at once.
//is val going to be a decimal equivalent or a bit string?
//naievly assume that the len(bit_string) is the size of the registers
void reg_assign(reg *r, char *bit_string)
{
  //should include loop that iterates over bit_string in char size chunks and assigns them memory allocated to the register
  int remaining = r -> size;
  for (int i = 0; i < r -> size; i += 8)
    {
      int s = (remaining % 8 == 0) ? 8: remaining % 8;
      char *temp = malloc(s);
      memcpy(temp, bit_string, s);
      r -> vals[(r -> size - i)/8] = strtol(temp, 0, 2);
      bit_string += s;
      remaining -= s;
      free(temp);
    }
}

int reg_access(reg *r, int bit_num) // lsb has bit_num = 0
{
  return (r -> vals)[bit_num/8] >> (bit_num % 8) & 0x01;
}

void print_register(reg *r)
{
  for (int i = r -> size - 1; i >= 0; i--)
    (i % 4 == 0) ? printf("%d ", reg_access(r, i)): printf("%d", reg_access(r, i));
}

void del_register(reg *r)
{
  free(r);
  free(r -> vals);
}
