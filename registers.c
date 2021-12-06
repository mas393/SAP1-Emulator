#include <stdio.h>
#include <stdlib.h>
#include "registers.h"
#include <string.h>

reg* init_register(int s)
{
  reg *r = malloc(sizeof(reg));
  r -> size = s;
  r -> vals = malloc(s/8+1);
  return r;
}

void reg_assign(reg *r, char *bit_string)
{
  for (int i = 0; i < r -> size; i += 8) *(r -> vals + i/8) = (strtol(bit_string, 0, 2) >> i) & 0xff;
}

int reg_access(reg *r, int bit_num) // lsb has bit_num = 0
{
  return (r -> vals)[bit_num/8] >> (bit_num % 8) & 0x01;
}

char* get_reg(reg *r, int s, int offset) 
{
  char *out = malloc(s);
  for (int i = s - 1; i >= 0; i--) out[s - 1 - i] = reg_access(r, i-offset) ? '1': '0';
  *(out + s) = '\0';
  return out;
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
