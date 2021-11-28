#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RAM.h"
#include "adder_subtracter.h"

//perhaps we abstract reg struct and functions as "memory" and then we build register and ram structs that incorporate the memory
// then the register struct could include enable load and clock bits
#define PCMAX 15;
typedef int ProgramCounter;
typedef int clock;
typedef reg bus;



enum control_word_bits {
  Load_O_bar,
  Load_B_bar,
  Enable_U,
  S_U,
  Enable_A,
  Load_A_bar,
  Enable_I_bar,
  Load_I_bar,
  CE_bar,
  Load_M_bar,
  Enable_P,
  C_P
};

enum ring_counter {T1, T2, T3, T4, T5, T6};
  
typedef struct controller_sequencer
{
  enum ring_counter rc;
  reg *control_word;
} controller_sequencer;

controller_sequencer* init_controller_sequencer(void)
{
  controller_sequencer *cs;
  cs -> rc = T1;
  return cs;
}

typedef struct computer
{
  ProgramCounter PC;
  clock clk;
  reg *Accumulator;
  reg *BReg;
  reg *InstructionReg;
  RAM *Mem;
  bus *WBus;
  bus *ControlBus;
  controller_sequencer *c_s;

  //input/mar
  //adder/subtracter
} computer;

int boot_computer(computer *c)
{
  c -> PC = 0;
  c -> clk = 0;
  c -> Accumulator = init_register(8);
  c -> BReg = init_register(8);
  c -> InstructionReg = init_register(8);
  c -> Mem = init_RAM(16, 8);
  c -> WBus = init_register(8);
  c -> ControlBus = init_register(12);

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
  printf("HELLO");

  //positive edge the blocks react to the control word
  

  
  //negative edge is clock incrementing
  
}

int run_program(computer *c)
{
  for ((c -> PC) < PCMAX; clock_tick(c)) //perhaps we want to control the clock ticks
    {
      printf("YAHOO");
    }

  return 0;
}
  
int main()
{
  //perhaps place in init_computer function

  bus *Wbus = init_register(8); //perhaps we should abstract the bus as a register that is always accessable

  reg *a = init_register(8);
  reg *b = init_register(8);
  reg_assign(a, "10101010");
  reg_assign(b, "10101010");
  adder_subtracter as;
  as.BReg = b;
  as.Accumulator = a;
  subtraction(&as);
    
  

  /*
  RAM *test = init_RAM(16, 8);
  RAM_assign(test, "0010", "00000001");
  RAM_assign(test, "1101", "10100101");
  print_RAM(test);
  */
  
  int status = 0;
  computer *SAP1;
  /*
  status = boot_computer(SAP1);
  status = load_instructions("filename", SAP1);
  status = run_program(SAP1);
  status = shutdown_computer(SAP1);
  
  
  
  reg_assign(Accumulator, "01011010");
  print_register(Accumulator);
  reg_assign(Accumulator, "1111"); //right now the behaviour overwrites non-specified digits with zeros
  print_register(Accumulator);
  */
  return 0;
}
