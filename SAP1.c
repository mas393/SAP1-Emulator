#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RAM.h"
#include "adder_subtracter.h"

//perhaps we abstract reg struct and functions as "memory" and then we build register and ram structs that incorporate the memory
// then the register struct could include enable load and clock bits
#define PCMAX 15
typedef int ProgramCounter;
typedef int clock; // do we need a clock?
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
      val[Enable_U] = '1';
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

typedef struct computer
{
  ProgramCounter PC;
  reg *Accumulator;
  reg *BReg;
  reg *InstructionReg;
  reg *Mar;
  RAM *Mem;
  bus *WBus;
  bus *ControlBus;
  adder_subtracter *as;
  controller_sequencer *c_s;

  //input/mar
} computer;

int boot_computer(computer *c)
{
  c -> PC = 0;
  c -> Accumulator = init_register(8);
  c -> BReg = init_register(8);
  c -> InstructionReg = init_register(8);
  c -> Mar = init_register(4);
  c -> Mem = init_RAM(16, 8);
  c -> WBus = init_register(8);
  c -> ControlBus = init_register(12);
  c -> as = init_adder_subtracter();
  c -> as -> Accumulator = c -> Accumulator;
  c -> as -> BReg = c -> BReg;
  c -> c_s = init_controller_sequencer();

  return 0;
}

int shutdown_computer(computer *c)
{
  del_register(c -> Accumulator);
  del_register(c -> BReg);
  del_register(c -> InstructionReg);
  free(c);
  return 0;
}

int load_instructions(char *f, computer *c)
{
  //will parse assembly in f and store in c -> mem
  return 0;
}

void clock_tick(computer *c)
{
  
  //need a upward clock edge actions and a downward clock edge actions
  printf("tick: control_bus = ");
  print_register(c->ControlBus);
  printf("\n");
  c -> PC++;

  //positive edge the blocks react to the control word
  
  //negative edge is clock incrementing
  
}

int machine_cycle(computer *c, char *instruction) //goes through ring counter
{
  int status = 0;
  printf("machine_cyle: ring counter = %d\n", c -> c_s -> rc);
    
  if (c -> c_s -> rc < T4) reg_assign(c -> ControlBus, get_control_word_fetch(c -> c_s));
  else
    {
      //need to replace instruction with something
      if (!strcmp(instruction, "1111")) return 1;// halt
      else if (!strcmp(instruction, "0000")) //lda
	{
	  reg_assign(c -> ControlBus, get_control_word_lda(c -> c_s));
	}
      else if (!strcmp(instruction, "0001")) //add
	{
	  reg_assign(c -> ControlBus, get_control_word_add(c -> c_s));
	}
      else if (!strcmp(instruction, "0010")) //sub
	{
	  reg_assign(c -> ControlBus, get_control_word_sub(c -> c_s));
	}
      else if (!strcmp(instruction, "1110")) //Out
	{
	  reg_assign(c -> ControlBus, get_control_word_out(c -> c_s));
	}
      else printf("should not be here machine_cycle");

    }
  clock_tick(c); //things happen on upward clock edge
  (c -> c_s -> rc == T6) ? c -> c_s -> rc = T1: c -> c_s -> rc++;
  return status;
}

int run_program(computer *c)
{
  while (c -> PC < PCMAX) //perhaps we want to control the clock ticks
    {
      printf("run program: PC = %d\n", c->PC);
      if (machine_cycle(c, "0010")) break;
    }
  return 0;
}
  
int main()
{
  //perhaps place in init_computer function

  //  bus *Wbus = init_register(8); //perhaps we should abstract the bus as a register that is always accessable
  /*
  reg *a = init_register(8);
  reg *b = init_register(8);
  reg_assign(a, "10101010");
  reg_assign(b, "10101010");
  adder_subtracter as;
  as.BReg = b;
  as.Accumulator = a;
  subtraction(&as);
  */
  //  ProgramCounter pc = 14;
  //  printf("%s", get_PC(pc));
  

  /*
  RAM *test = init_RAM(16, 8);
  RAM_assign(test, "0010", "00000001");
  RAM_assign(test, "1101", "10100101");
  print_RAM(test);
  */
  
  int status = 0;
  computer *SAP1 = malloc(sizeof(computer)); 
  
  status = boot_computer(SAP1);
  //  status = load_instructions("filename", SAP1);
  status = run_program(SAP1);
  //  status = shutdown_computer(SAP1);
  
  
  /*
  reg_assign(Accumulator, "01011010");
  print_register(Accumulator);
  reg_assign(Accumulator, "1111"); //right now the behaviour overwrites non-specified digits with zeros
  print_register(Accumulator);
  */
  return 0;
}
