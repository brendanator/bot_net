#include "transposition.h"

void init_move_generation();
Position new_game();
Move new_move();

void place_piece(Position *position, PlacePiece piece);
void make_move(Position *position, Move move);
void make_step(Position *position, Step step, int step_number);

Move find_best_move(Position position);
int generate_single_steps(Position position, Move current_move, Move *moves);
int generate_push_steps(Position position, Move current_move, Move *moves);
int generate_pull_steps(Position position, Move current_move, Move *moves);

Square first_square(Bitboard board);
Type type_at_square(Position position, Colour colour, Square square);
Bitboard bitboard_at(Square square);
Bitboard square_neighbours[SQUARE_COUNT];

void print_position(Position position);
void print_short_position(Position position);
void print_bitboard(Bitboard bitboard);
void print_move(Position position, Move move);
