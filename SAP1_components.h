#ifndef SAP1_COMPONENTS_H
#define SAP1_COMPONENTS_H

#include "registers.h"

#define CONTROL_WORD "001111100011"
#define LDA "0000"
#define ADD "0001"
#define SUB "0010"
#define OUT "1110"
#define HLT "1111"

typedef struct controller_sequencer
{
  //  enum ring_counter rc;
  int rc;
  reg *control_word;
  reg *instruction;
} controller_sequencer;

controller_sequencer* init_controller_sequencer();
//void del_controller_sequencer(controller_sequencer*);

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

void get_control_word_fetch(controller_sequencer*, char*);
void get_control_word_lda(controller_sequencer*, char*);
void get_control_word_add(controller_sequencer*, char*);
void get_control_word_sub(controller_sequencer*, char*);
void get_control_word_out(controller_sequencer*, char*);

void bit_string_from_int(int, int, char*);


typedef int ProgramCounter;

void get_PC(ProgramCounter, char*);

  
typedef reg bus;


typedef struct InstructionReg
{
  reg *lowNibble;
  reg *highNibble;
} InstructionReg;

InstructionReg* init_InstructionReg();
void del_InstructionReg(InstructionReg*);

typedef struct adder_subtracter
{
  reg *Accumulator;
  reg *BReg;
} adder_subtracter;

adder_subtracter* init_adder_subtracter();
//void del_adder_subtracter(adder_subtracter*);

char add_bits(reg*, reg*, int, char*);

void addition(adder_subtracter*, char*);

void subtraction(adder_subtracter*, char*);


#endif
