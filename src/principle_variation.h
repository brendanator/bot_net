#ifndef PRINCIPLE_VARIATION_H
#define PRINCIPLE_VARIATION_H

#include "types.h"

typedef struct PrincipleVariation {
  int move_count;
  Move move[20];
} PrincipleVariation;

void clear_principle_variation();
void update_principle_variation(Move move, int ply);
void restore_principle_variation_to_transposition_table(Position position);
PrincipleVariation principle_variation(); 

#endif
