#ifndef KILLER_H
#define KILLER_H

#include "types.h"

void clear_killers();
int add_killers(Position position, Move current_move, int ply, Move moves[]);
void save_killer(Move move, int ply);

#endif
