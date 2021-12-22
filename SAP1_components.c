#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SAP1_components.h"


controller_sequencer* init_controller_sequencer()
{
  controller_sequencer *cs;
  cs = malloc(sizeof(controller_sequencer));
  cs -> rc = 0;
  //T1;
  return cs;
}

char* get_control_word_fetch(controller_sequencer *cs)
{
  char *cw = malloc(12);
  memcpy(cw, CONTROL_WORD, 12);

  switch (cs -> rc)
    {
    case 0:
      cw[Enable_P] = '1';
      cw[Load_M_bar] = '0';
      break;
    case 1:
      cw[C_P] = '1';
      break;
    case 2:
      cw[CE_bar] = '0';
      cw[Load_I_bar] = '0';
      break;
    default:
      printf("should not be here control_word_fetch");
    };
  return cw;
}

char* get_control_word_lda(controller_sequencer *cs)
{
  char *cw = malloc(12);
  memcpy(cw, CONTROL_WORD, 12); //default control word with everything

  switch (cs -> rc)
    {
    case 3:
      cw[Load_M_bar] = '0';
      cw[Enable_I_bar] = '0';
      break;
    case 4:
      cw[CE_bar] = '0';
      cw[Load_A_bar] = '0';
      break;
    case 5: //nop
      break;
    default:
      printf("should not be here lda");
    }
  
  return cw;  
}

char* get_control_word_add(controller_sequencer *cs)
{
  char *cw = malloc(12);
  memcpy(cw, CONTROL_WORD, 12); //default control word with everything

    switch (cs -> rc)
    {
    case 3:
      cw[Load_M_bar] = '0';
      cw[Enable_I_bar] = '0';
      break;
    case 4:
      cw[CE_bar] = '0';
      cw[Load_B_bar] = '0';
      break;
    case 5:
      cw[Load_A_bar] = '0';
      cw[Enable_U] = '1';
      break;
    default:
      printf("should not be here add");
    }
    
  return cw;  
}

char* get_control_word_sub(controller_sequencer *cs)
{
  char *cw = malloc(12);
  memcpy(cw, CONTROL_WORD, 12); //default control word with everything

    switch (cs -> rc)
    {
    case 3:
      cw[Load_M_bar] = '0';
      cw[Enable_I_bar] = '0';
      break;
    case 4:
      cw[CE_bar] = '0';
      cw[Load_B_bar] = '0';
      break;
    case 5: 
      cw[Load_A_bar] = '0';
      cw[S_U] = '1';
      break;
    default:
      printf("should not be here sub");
    }
    
  return cw;  
}

char* get_control_word_out(controller_sequencer *cs)
{
  char *cw = malloc(12);
  memcpy(cw, CONTROL_WORD, 12); //default control word with everything

  switch (cs -> rc)
    {
    case 3:
      cw[Enable_A] = '1';
      cw[Load_O_bar] = '0';
      break;
    case 4: //nop
      break;
    case 5: //nop
      break;
    default:
      printf("should not be here out");
    }
  
  return cw;
}

char* bit_string_from_int(int in, int s)
{
  char *val = malloc(s);
  for (int i = 0; i < s; i++) val[s-i-1] = ((char)in >> i & 0x1 ? '1': '0');
  return val;			
}
char* get_PC(ProgramCounter PC)
{
  return bit_string_from_int(PC, 4);
}

InstructionReg* init_InstructionReg()
{
  InstructionReg *temp;
  temp = malloc(sizeof(InstructionReg));
  temp -> lowNibble = init_register(4);
  temp -> highNibble = init_register(4);
  return temp;
}

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
