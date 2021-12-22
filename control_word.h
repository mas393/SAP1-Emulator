#ifndef CONTROL_WORD_H
#define CONTROL_WORD_H

#include "SAP1_components.h"

#define CONTROL_WORD "001111100011"
#define LDA "0000"
#define ADD "0001"
#define SUB "0010"
#define OUT "1110"
#define HLT "1111"

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

char* get_control_word_fetch(controller_sequencer*);
char* get_control_word_add(controller_sequencer*);
char* get_control_word_sub(controller_sequencer*);
char* get_control_word_out(controller_sequencer*);

#endif
