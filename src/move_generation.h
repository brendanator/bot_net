#ifndef MOVE_GENERATION_H
#define MOVE_GENERATION_H

#include "types.h"

void init_move_generation();

Position new_game();
Move new_move();

void place_piece(Position *position, PlacePiece piece);
void make_move(Position *position, Move move);
void make_step(Position *position, Step step, int step_number);

int generate_pass(Move current_move, Move *moves);
int generate_single_steps(Position position, Move current_move, Move *moves);
int generate_push_pull_steps(Position position, Move current_move, Move *moves);

int generate_moves(Position position, Move current_move, Move moves[], int move_count);

bool is_legal(Position position, Move move);

#endif // MOVE_GENERATION_H