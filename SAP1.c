#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "registers.h"
//#include "RAM.h"
#include "adder_subtracter.h"

#define PCMAX 15

// TODO: reorganize .h and .c hierarchy 
// TODO: Perhaps implement a constant string that contains the base control word
// TODO: implement computer shutdown function to free all components

typedef int ProgramCounter;
typedef reg bus;

char* get_PC(ProgramCounter PC)
{
  char *val = malloc(4);
  for (int i = 0; i < 4; i++) val[4-i-1] = ((char)PC >> i & 0x1 ? '1': '0');
  return val;			
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
  
  char *val = malloc(12);
  memcpy(val, "001111100011", 12);  //default control word with everything 
  switch (cs -> rc)
    {
    case T1:
      val[Enable_P] = '1';
      val[Load_M_bar] = '0';
      break;
    case T2:
      val[C_P] = '1';
      break;
    case T3:
      val[CE_bar] = '0';
      val[Load_I_bar] = '0';
      break;
    default:
      printf("should not be here control_word_fetch");
    };
  return val;
}

char* get_control_word_lda(controller_sequencer *cs)
{
  char *val = malloc(12);
  memcpy(val, "001111100011", 12); //default control word with everything

  switch (cs -> rc)
    {
    case T4:
      val[Load_M_bar] = '0';
      val[Enable_I_bar] = '0';
      break;
    case T5:
      val[CE_bar] = '0';
      val[Load_A_bar] = '0';
      break;
    case T6: //nop
      break;
    default:
      printf("should not be here lda");
    }
  
  return val;  
}

char* get_control_word_add(controller_sequencer *cs)
{
  char *val = malloc(12);
  memcpy(val, "001111100011", 12); //default control word with everything

    switch (cs -> rc)
    {
    case T4:
      val[Load_M_bar] = '0';
      val[Enable_I_bar] = '0';
      break;
    case T5:
      val[CE_bar] = '0';
      val[Load_B_bar] = '0';
      break;
    case T6:
      val[Load_A_bar] = '0';
      val[Enable_U] = '1';
      break;
    default:
      printf("should not be here add");
    }
    
  return val;  
}

char* get_control_word_sub(controller_sequencer *cs)
{
  char *val = malloc(12);
  memcpy(val, "001111100011", 12); //default control word with everything

    switch (cs -> rc)
    {
    case T4:
      val[Load_M_bar] = '0';
      val[Enable_I_bar] = '0';
      break;
    case T5:
      val[CE_bar] = '0';
      val[Load_B_bar] = '0';
      break;
    case T6: 
      val[Load_A_bar] = '0';
      val[S_U] = '1';
      break;
    default:
      printf("should not be here sub");
    }
    
  return val;  
}

char* get_control_word_out(controller_sequencer *cs)
{
  char *val = malloc(12);
  memcpy(val, "001111100011", 12); //default control word with everything

  switch (cs -> rc)
    {
    case T4:
      val[Enable_A] = '1';
      val[Load_O_bar] = '0';
      break;
    case T5: //nop
      break;
    case T6: //nop
      break;
    default:
      printf("should not be here out");
    }
  
  return val;
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
  c -> Mem -> cur_addr = c -> Mar; //
  c -> WBus = init_register(8);
  c -> ControlBus = init_register(12);
  c -> as = init_adder_subtracter();
  c -> as -> Accumulator = c -> Accumulator;
  c -> as -> BReg = c -> BReg;
  c -> c_s = init_controller_sequencer();
  c -> c_s -> instruction = c -> ir -> highNibble;

  return 0;
}

/*
int shutdown_computer(computer *c)
{
  del_register(c -> Accumulator);
  del_register(c -> BReg);
  del_register(c -> InstructionReg);
  free(c);
  return 0;
}
*/

int load_instructions(char *f, computer *c)
{
  //instructions
  set_RAM(c -> Mem, "0000", "00001001"); //lda 9h
  set_RAM(c -> Mem, "0001", "00011010"); //add ah
  set_RAM(c -> Mem, "0010", "00011011"); //add bh
  set_RAM(c -> Mem, "0011", "00101100"); //sub ch
  set_RAM(c -> Mem, "0100", "11100000"); //out
  set_RAM(c -> Mem, "0101", "11110000"); //hlt
  //data
  set_RAM(c -> Mem, "1001", "00010000");
  set_RAM(c -> Mem, "1010", "00010100");
  set_RAM(c -> Mem, "1011", "00011000");
  set_RAM(c -> Mem, "1100", "00100000");
  //will parse assembly in f and store in c -> mem
  return 0;
}


int control_string_change(computer *c, int b)
{
  char val[] = "001111100011";
  char test = reg_access(c -> ControlBus, c -> ControlBus -> size - b - 1) ? '1':'0';
  return (test != val[b]);
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
      if (!strcmp(get_reg(c ->c_s -> instruction, 4, 0), "1111")) return 1;// halt
      else if (!strcmp(get_reg(c ->c_s -> instruction, 4, 0), "0000")) //lda
	{
	  reg_assign(c -> ControlBus, get_control_word_lda(c -> c_s));
	}
      else if (!strcmp(get_reg(c ->c_s -> instruction, 4, 0), "0001")) //add
	{
	  reg_assign(c -> ControlBus, get_control_word_add(c -> c_s));
	}
      else if (!strcmp(get_reg(c ->c_s -> instruction, 4, 0), "0010")) //sub
	{
	  reg_assign(c -> ControlBus, get_control_word_sub(c -> c_s));
	}
      else if (!strcmp(get_reg(c ->c_s -> instruction, 4, 0), "1110")) //Out
	{
	  reg_assign(c -> ControlBus, get_control_word_out(c -> c_s));
	}
      else printf("Error in machine cycle: no matching instruction found\n");

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
  while (c -> PC < PCMAX) //perhaps we want to control the clock ticks
    {
      if (machine_cycle(c )) break;
    }
  return 0;
}
  
int main()
{
  /*
  reg *a = init_register(8);
  reg *b = init_register(8);
  reg_assign(a, "00111100");
  reg_assign(b, "00100000");
  adder_subtracter as;
  as.BReg = b;
  as.Accumulator = a;
  printf("out %s\n", addition(&as));
  */
  int status = 0;
  computer *SAP1 = malloc(sizeof(computer)); 
  
  status = boot_computer(SAP1);
  status = load_instructions("filename", SAP1);
  status = run_program(SAP1);
  //  status = shutdown_computer(SAP1);

  return 0;
}
