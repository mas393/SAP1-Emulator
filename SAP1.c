#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "registers.h"
//#include "RAM.h"
#include "adder_subtracter.h"

#define PCMAX 15
#define CONTROL_WORD "001111100011"
#define LDA "0000"
#define ADD "0001"
#define SUB "0010"
#define OUT "1110"
#define HLT "1111"

// TODO: reorganize .h and .c hierarchy 
// TODO: implement computer shutdown function to free all components and check in valgrind

typedef int ProgramCounter;
typedef reg bus;

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

enum control_word_bits {
  C_P,
  Enable_P,
  Load_M_bar,
  CE_bar,
  Load_I_bar,
  Enable_I_bar,
  Load_A_bar,
  Enable_A,
  S_U,
  Enable_U,
  Load_B_bar,
  Load_O_bar
};

enum ring_counter {T1, T2, T3, T4, T5, T6};
  
typedef struct controller_sequencer
{
  enum ring_counter rc;
  reg *control_word;
  reg *instruction;
} controller_sequencer;

controller_sequencer* init_controller_sequencer()
{
  controller_sequencer *cs;
  cs = malloc(sizeof(controller_sequencer));
  cs -> rc = T1;
  return cs;
}

char* get_control_word_fetch(controller_sequencer *cs)
{
  char *cw = malloc(12);
  memcpy(cw, CONTROL_WORD, 12);

  switch (cs -> rc)
    {
    case T1:
      cw[Enable_P] = '1';
      cw[Load_M_bar] = '0';
      break;
    case T2:
      cw[C_P] = '1';
      break;
    case T3:
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
    case T4:
      cw[Load_M_bar] = '0';
      cw[Enable_I_bar] = '0';
      break;
    case T5:
      cw[CE_bar] = '0';
      cw[Load_A_bar] = '0';
      break;
    case T6: //nop
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
    case T4:
      cw[Load_M_bar] = '0';
      cw[Enable_I_bar] = '0';
      break;
    case T5:
      cw[CE_bar] = '0';
      cw[Load_B_bar] = '0';
      break;
    case T6:
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
    case T4:
      cw[Load_M_bar] = '0';
      cw[Enable_I_bar] = '0';
      break;
    case T5:
      cw[CE_bar] = '0';
      cw[Load_B_bar] = '0';
      break;
    case T6: 
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
    case T4:
      cw[Enable_A] = '1';
      cw[Load_O_bar] = '0';
      break;
    case T5: //nop
      break;
    case T6: //nop
      break;
    default:
      printf("should not be here out");
    }
  
  return cw;
}


typedef struct InstructionReg
{
  reg *lowNibble;
  reg *highNibble;
} InstructionReg;

InstructionReg* init_InstructionReg()
{
  InstructionReg *temp;
  temp = malloc(sizeof(InstructionReg));
  temp -> lowNibble = init_register(4);
  temp -> highNibble = init_register(4);
  return temp;
}

typedef struct computer
{
  ProgramCounter PC;
  reg *Accumulator;
  reg *BReg;
  reg *Mar;
  InstructionReg *ir;
  RAM *Mem;
  bus *WBus;
  bus *ControlBus;
  adder_subtracter *as;
  controller_sequencer *c_s;
} computer;

void print_state(computer *c)
{
  printf("\n");
  printf("-----------PROGRAM STATE------------\n");
  printf("program counter = %d\n", c -> PC);
  printf("Accumulator = "); print_register(c -> Accumulator); printf("\n");
  printf("BReg = "); print_register(c -> BReg); printf("\n");
  printf("Mar = "); print_register(c -> Mar); printf("\n");
  printf("Instruction Register: \n");
  printf("  High nibble = "); print_register(c -> ir -> highNibble); printf("\n");
  printf("  Low nibble  = "); print_register(c -> ir -> lowNibble); printf("\n");
  printf("\n");
}

int boot_computer(computer *c)
{
  c -> PC = 0;
  c -> Accumulator = init_register(8);
  c -> BReg = init_register(8);
  c -> ir = init_InstructionReg();
  c -> Mar = init_register(4);
  c -> Mem = init_RAM(16, 8);
  c -> Mem -> cur_addr = c -> Mar; 
  c -> WBus = init_register(8);
  c -> ControlBus = init_register(12);
  c -> as = init_adder_subtracter();
  c -> as -> Accumulator = c -> Accumulator;
  c -> as -> BReg = c -> BReg;
  c -> c_s = init_controller_sequencer();
  c -> c_s -> instruction = c -> ir -> highNibble;

  return 0;
}


int shutdown_computer(computer *c)
{
  del_register(c -> Accumulator);
  del_register(c -> BReg);
  //  del_register(c -> ir);
  del_register(c -> Mar);
  del_RAM(c -> Mem);
  del_register(c -> WBus);
  del_register(c -> ControlBus);
  free(c);
  return 0;
}


int load_instructions(char *f, computer *c)
{
  FILE *fin = fopen(f, "r");

  char *line = malloc(20);
  
  while (fgets(line, 20, fin))
    {
      int loc = 0;
      int data = 0;
      char *instruction = malloc(3);

      
      sscanf(line, "%xH %s %xH\n", &loc, instruction, &data);
      
      char *d  = bit_string_from_int(data, 4);
      char e[4];
      
      if (!strcmp(instruction, "LDA")) memcpy(e, LDA, 4); 
      else if (!strcmp(instruction, "ADD")) memcpy(e, ADD, 4); 
      else if (!strcmp(instruction, "SUB")) memcpy(e, SUB, 4); 
      else if (!strcmp(instruction, "OUT")) memcpy(e, OUT, 4); 
      else if (!strcmp(instruction, "HLT")) memcpy(e, HLT, 4); 
      else
	{
	  
	  sscanf(instruction, "%xH", &data);
	  d = bit_string_from_int(data, 8);
	  memcpy(e, "0000", 4); //set to 0000 so set_Ram works with e and d vals
	  printf("data in %s\n", d);
	}
      
      set_RAM(c -> Mem, bit_string_from_int(loc, 4), e,d);      
    }

  fclose(fin);
  
  return 0;
}


int control_string_change(computer *c, int b)
{
  char test = reg_access(c -> ControlBus, c -> ControlBus -> size - b - 1) ? '1':'0';
  return (test != CONTROL_WORD[b]);
}

void clock_tick_up(computer *c)
{
  if (control_string_change(c, C_P)) c -> PC++;

  // set bus
  if (control_string_change(c, Enable_P)) reg_assign(c -> WBus, get_PC(c -> PC));
  else if (control_string_change(c, CE_bar)) reg_assign(c -> WBus, get_RAM(c -> Mem));
  else if (control_string_change(c, Enable_I_bar)) reg_assign(c -> WBus, get_reg(c -> ir -> lowNibble, 4, 0));
  else if (control_string_change(c, Enable_U)) reg_assign(c -> WBus, addition(c -> as)); //could just return char from additon/subtraction
  else if (control_string_change(c, S_U)) reg_assign(c -> WBus, subtraction(c -> as)); //could just return char from additon/subtraction
  else if (control_string_change(c, Enable_A)) reg_assign(c -> WBus, get_reg(c -> Accumulator, 8, 0));

  //  get bus
  if (control_string_change(c, Load_M_bar)) reg_assign(c -> Mar, get_reg(c -> WBus, 4, 0));
  else if (control_string_change(c, Load_I_bar))
    {
      reg_assign(c -> ir -> highNibble, get_reg(c -> WBus, 4, 4));
      reg_assign(c -> ir -> lowNibble, get_reg(c -> WBus, 4, 0));
    }
  else if (control_string_change(c, Load_A_bar)) reg_assign(c -> Accumulator, get_reg(c -> WBus, 8, 0));
  else if (control_string_change(c, Load_B_bar)) reg_assign(c -> BReg, get_reg(c -> WBus, 8, 0));
  else if (control_string_change(c, Load_O_bar))
    {
      printf("\n");
      printf("***********PROGRAM OUTPUT************\n");
      print_register(c -> WBus);
      printf("\n\n");
    }					      
}

int machine_cycle(computer *c) //goes through ring counter
{
  int status = 0;
    
  if (c -> c_s -> rc < T4) reg_assign(c -> ControlBus, get_control_word_fetch(c -> c_s));

  else
    {
      if (!strcmp(get_reg(c ->c_s -> instruction, 4, 0), HLT)) return 1;
      else if (!strcmp(get_reg(c ->c_s -> instruction, 4, 0), LDA))
	reg_assign(c -> ControlBus, get_control_word_lda(c -> c_s));
      else if (!strcmp(get_reg(c ->c_s -> instruction, 4, 0), ADD))  
	reg_assign(c -> ControlBus, get_control_word_add(c -> c_s));
      else if (!strcmp(get_reg(c ->c_s -> instruction, 4, 0), SUB)) 
	reg_assign(c -> ControlBus, get_control_word_sub(c -> c_s));
      else if (!strcmp(get_reg(c ->c_s -> instruction, 4, 0), OUT)) 
	reg_assign(c -> ControlBus, get_control_word_out(c -> c_s));
      else printf("Error in machine cycle: no matching instruction found for %s\n", get_reg(c->c_s-> instruction, 4, 0));
      
    }

  printf("T = %d:\n", c->c_s->rc+1);
  printf("  Control Bus: "); print_register(c -> ControlBus); printf("\n");

  clock_tick_up(c);
  printf("  W Bus:       "); print_register(c -> WBus); printf("\n");

  //increment ring counter
  if (c -> c_s -> rc == T6)
    {
      c -> c_s -> rc = T1;
      print_state(c);
    }
  else c -> c_s -> rc++; 

  return status;
}

int run_program(computer *c)
{
  //print_RAM(c -> Mem);
  while (c -> PC < PCMAX) //perhaps we want to control the clock ticks
    {
      if (machine_cycle(c )) break;
    }
  return 0;
}
  
int main()
{

  int status = 0;
  computer *SAP1 = malloc(sizeof(computer)); 
  
  status = boot_computer(SAP1);
  status = load_instructions("test.txt", SAP1);
  status = run_program(SAP1);
  //  status = shutdown_computer(SAP1);

  return 0;
}
