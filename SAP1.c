#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "registers.h"
#include "RAM.h"
#include "SAP1_components.h"

#define PCMAX 15

// TODO: write readme and comment functions
// TODO: look into using bit fields
// TODO: rewrite functions that return char* to modify existing char* array in place
// TODO: fix valgrind errors

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


void print_state(computer*);
int boot_computer(computer*);
int shutdown_computer(computer*);
int load_instructions(char*, computer*);
int control_string_change(computer*, int);
void clock_tick_up(computer*);
int machine_cycle(computer*);
int run_program(computer*);
  
int main()
{
  computer *SAP1 = malloc(sizeof(computer)); 

  boot_computer(SAP1);
  load_instructions("test.txt", SAP1);
  run_program(SAP1);
  shutdown_computer(SAP1);
  
  return 0;
}

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
  del_InstructionReg(c -> ir);
  del_register(c -> Mar);
  del_RAM(c -> Mem);
  del_register(c -> WBus);
  del_register(c -> ControlBus);
  free(c -> as);
  free(c -> c_s);
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
	  free(d);
	  sscanf(instruction, "%xH", &data);
	  d = bit_string_from_int(data, 8);
	  memcpy(e, "0000", 4); //set to 0000 so set_Ram works with e and d vals
	}

      char *mem_loc = bit_string_from_int(loc, 4);
      set_RAM(c -> Mem, mem_loc, e,d);
      free(instruction);
      free(mem_loc);
      free(d);
    }

  fclose(fin);
  free(line);
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
  if (control_string_change(c, Enable_P)) reg_assign(c -> WBus, get_PC(c -> PC), 1);
  else if (control_string_change(c, CE_bar)) reg_assign(c -> WBus, get_RAM(c -> Mem), 1);
  else if (control_string_change(c, Enable_I_bar)) reg_assign(c -> WBus, get_reg(c -> ir -> lowNibble, 4, 0), 1);
  else if (control_string_change(c, Enable_U)) reg_assign(c -> WBus, addition(c -> as), 1); //could just return char from additon/subtraction
  else if (control_string_change(c, S_U)) reg_assign(c -> WBus, subtraction(c -> as), 1); //could just return char from additon/subtraction
  else if (control_string_change(c, Enable_A)) reg_assign(c -> WBus, get_reg(c -> Accumulator, 8, 0), 1);

  //  get bus
  if (control_string_change(c, Load_M_bar)) reg_assign(c -> Mar, get_reg(c -> WBus, 4, 0), 1);
  else if (control_string_change(c, Load_I_bar))
    {
      reg_assign(c -> ir -> highNibble, get_reg(c -> WBus, 4, 4), 1);
      reg_assign(c -> ir -> lowNibble, get_reg(c -> WBus, 4, 0), 1);
    }
  else if (control_string_change(c, Load_A_bar)) reg_assign(c -> Accumulator, get_reg(c -> WBus, 8, 0), 1);
  else if (control_string_change(c, Load_B_bar)) reg_assign(c -> BReg, get_reg(c -> WBus, 8, 0), 1);
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
    
  if (c -> c_s -> rc < 3) reg_assign(c -> ControlBus, get_control_word_fetch(c -> c_s), 1);

  else
    {
      char *ins = get_reg(c -> c_s -> instruction, 4, 0);
      if (!strcmp(ins, HLT))
	{
	  free(ins);
	  return 1;
	}
      else if (!strcmp(ins, LDA))
	reg_assign(c -> ControlBus, get_control_word_lda(c -> c_s), 1);
      else if (!strcmp(ins, ADD))  
	reg_assign(c -> ControlBus, get_control_word_add(c -> c_s), 1);
      else if (!strcmp(ins, SUB)) 
	reg_assign(c -> ControlBus, get_control_word_sub(c -> c_s), 1);
      else if (!strcmp(ins, OUT)) 
	reg_assign(c -> ControlBus, get_control_word_out(c -> c_s), 1);
      else printf("Error in machine cycle: no matching instruction found for %s\n", ins);
      free(ins);
    }

  printf("T = %d:\n", c->c_s->rc+1);
  printf("  Control Bus: "); print_register(c -> ControlBus); printf("\n");

  clock_tick_up(c);
  printf("  W Bus:       "); print_register(c -> WBus); printf("\n");

  //increment ring counter
  if (c -> c_s -> rc == 5)
    {
      c -> c_s -> rc = 0;
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
      if (machine_cycle(c)) break;
    }
  return 0;
}
