#ifndef PRINT_H
#define PRINT_H

#include "types.h"

void print_position(Position position);
void print_short_position(Position position);
void print_bitboard(Bitboard bitboard);
void print_step(Position position, Step step);
void print_move(Position position, Move move);

#endif //PRINT_H
