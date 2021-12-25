#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "registers.h"
#include "RAM.h"
#include "SAP1_components.h"

#define PCMAX 15

// TODO: write readme and comment functions
// TODO: look into using bit fields
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
void boot_computer(computer*);
void shutdown_computer(computer*);
void load_instructions(char*, computer*);
int control_string_change(computer*, int);
void clock_tick_up(computer*);
int machine_cycle(computer*);
void run_program(computer*);
  
int main(int argc, char** argv)
{
  if (argc < 2) {printf("Error: please include a file for SAP-1 to run\n"); return 1;}
  computer *SAP1 = malloc(sizeof(computer)); 
  boot_computer(SAP1);
  load_instructions(argv[1], SAP1);
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

void boot_computer(computer *c)
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
}

void shutdown_computer(computer *c)
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
}


void load_instructions(char *f, computer *c)
{
  FILE *fin = fopen(f, "r");

  char *line = malloc(20);
  //move char arrays etc out here and free below while loop
  int loc = 0;
  int data = 0;
  char *instruction = malloc(3);
  char *data_bitstring = malloc(4);
  char *opcode = malloc(4);
  char *mem_loc = malloc(4);
      
  while (fgets(line, 20, fin))
    {      
      sscanf(line, "%xH %s %xH\n", &loc, instruction, &data); 
      bit_string_from_int(data, 4, data_bitstring);
      
      if (!strcmp(instruction, "LDA")) memcpy(opcode, LDA, 4); 
      else if (!strcmp(instruction, "ADD")) memcpy(opcode, ADD, 4); 
      else if (!strcmp(instruction, "SUB")) memcpy(opcode, SUB, 4); 
      else if (!strcmp(instruction, "OUT")) memcpy(opcode, OUT, 4); 
      else if (!strcmp(instruction, "HLT")) memcpy(opcode, HLT, 4); 
      else
	{
	  sscanf(instruction, "%xH", &data);
	  bit_string_from_int(data, 8, data_bitstring);
	  memcpy(opcode, "0000", 4); //set to 0000 so set_Ram works with e and d vals
	}

      bit_string_from_int(loc, 4, mem_loc);
      set_RAM(c -> Mem, mem_loc, opcode, data_bitstring);
    }
  
  free(mem_loc);
  free(opcode);
  free(data_bitstring);
  free(instruction);
  free(line);
  
  fclose(fin);
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
  char *updated_bus = malloc(sizeof(c -> WBus));
  if (control_string_change(c, Enable_P)) get_PC(c -> PC, updated_bus);
  else if (control_string_change(c, CE_bar)) get_RAM(c -> Mem, updated_bus);
  else if (control_string_change(c, Enable_I_bar)) get_reg(c -> ir -> lowNibble, 4, 0, updated_bus);
  else if (control_string_change(c, Enable_U)) addition(c -> as, updated_bus);
  else if (control_string_change(c, S_U)) subtraction(c -> as, updated_bus);
  else if (control_string_change(c, Enable_A)) get_reg(c -> Accumulator, 8, 0, updated_bus);
  // else control string did not change
  reg_assign(c -> WBus, updated_bus);
  free(updated_bus);
  
  //  get bus
  if (control_string_change(c, Load_M_bar))
    {
      char *updated_reg = malloc(sizeof(c -> Mar));
      get_reg(c -> WBus, 4, 0, updated_reg);
      reg_assign(c -> Mar, updated_reg);
      free(updated_reg);
    }
  else if (control_string_change(c, Load_I_bar))
    {
      char *updated_reg = malloc(sizeof(c -> ir -> highNibble));
      get_reg(c -> WBus, 4, 4, updated_reg);
      reg_assign(c -> ir -> highNibble, updated_reg);
      get_reg(c -> WBus, 4, 0, updated_reg);
      reg_assign(c -> ir -> lowNibble, updated_reg);
      free(updated_reg);
    }
  else if (control_string_change(c, Load_A_bar))
    {
      char *updated_reg = malloc(sizeof(c -> Accumulator));
      get_reg(c -> WBus, 8, 0, updated_reg);
      reg_assign(c -> Accumulator, updated_reg);
      free(updated_reg);
    }
  else if (control_string_change(c, Load_B_bar))
    {
      char *updated_reg = malloc(sizeof(c -> BReg));
      get_reg(c -> WBus, 8, 0, updated_reg);
      reg_assign(c -> BReg, updated_reg);
      free(updated_reg);
    }
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
  char *updated_cw = malloc(sizeof(c -> ControlBus));
  if (c -> c_s -> rc < 3) get_control_word_fetch(c -> c_s, updated_cw);

  else
    {
      char *ins = malloc(sizeof(c -> c_s -> instruction));	
      get_reg(c -> c_s -> instruction, 4, 0, ins);
      if (!strcmp(ins, HLT))
	{
	  free(ins);
	  free(updated_cw);
	  return 1;
	}
      else if (!strcmp(ins, LDA)) get_control_word_lda(c -> c_s, updated_cw);
      else if (!strcmp(ins, ADD)) get_control_word_add(c -> c_s, updated_cw);
      else if (!strcmp(ins, SUB)) get_control_word_sub(c -> c_s, updated_cw);
      else if (!strcmp(ins, OUT)) get_control_word_out(c -> c_s, updated_cw);
      else printf("Error in machine cycle: no matching instruction found for %s\n", ins);
      free(ins);
    }
  reg_assign(c -> ControlBus, updated_cw);
  free(updated_cw);

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

  return 0;
}

void run_program(computer *c)
{
  //print_RAM(c -> Mem);
  while (c -> PC < PCMAX) //perhaps we want to control the clock ticks
    {
      if (machine_cycle(c)) break;
    }
}
