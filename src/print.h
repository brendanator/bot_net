#ifndef PRINT_H
#define PRINT_H

#include "types.h"
#include "principle_variation.h"
#include "transposition.h"

#include <stdio.h>

void print_turn(Position position);
void print_position(Position position);
void print_short_position(Position position);
void print_bitboard(Bitboard bitboard);
void print_step(Step step);
void print_steps(Move move);
void print_full_step(Position position, Step step);
void print_move(Position position, Move move);
void print_principle_variation(Position position, PrincipleVariation pv);
void print_transposition(Transposition transposition);

void print_stack_trace();

#endif //PRINT_H
