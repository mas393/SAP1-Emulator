#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "control_word.h"

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
