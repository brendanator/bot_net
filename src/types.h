#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef enum Colour { GOLD, SILVER, COLOUR_COUNT } Colour;
Colour turn_colour(int turn);
Colour enemy_colour(Colour colour);

typedef enum Type { ALL, RABBIT, CAT, DOG, HORSE, CAMEL, ELEPHANT, TYPE_COUNT } Type;

typedef uint8_t Square;
#define SQUARE_COUNT 64

// The Direction enum is ordered such that EMPTY_STEP and PASS_STEP are not valid steps
typedef enum Direction { SOUTH, NORTH, WEST, EAST } Direction;

// Indexed so that - a1 = 0, h1 = 7, a8 = 56, h8 = 63
typedef uint64_t Bitboard;
Bitboard north(Bitboard bitboard);
Bitboard south(Bitboard bitboard);
Bitboard east(Bitboard bitboard);
Bitboard west(Bitboard bitboard);
int population(Bitboard bitboard);

// Useful bitboards
#define NOT_A_FILE  0xfefefefefefefefeULL
#define NOT_H_FILE  0x7f7f7f7f7f7f7f7fULL
#define NOT_1_RANK  0xffffffffffffff00ULL
#define NOT_8_RANK  0x00ffffffffffffffULL
#define GOLD_GOAL   0xff00000000000000ULL
#define SILVER_GOAL 0x00000000000000ffULL
#define TRAPS       0x0000240000240000ULL
#define EMPTY       0ULL

typedef uint64_t Hash;

typedef enum Winner { GOLD_WIN = 1, NO_WINNER = 0, SILVER_WIN = -1} Winner;
typedef struct Position {
  Bitboard pieces[COLOUR_COUNT][TYPE_COUNT];
  int turn;
  int steps;
  Winner winner;
  Hash hash;
} Position;
Position new_game();

typedef struct PlacePiece {
  Colour colour;
  Type type;
  Square square;
} PlacePiece;

typedef uint8_t Step;
Step new_step(Square from, Square to);
Square step_from(Step step);
Square step_to(Step step);
Direction step_direction(Step step);
#define EMPTY_STEP 0
#define PASS_STEP 0xff
#define STEP_COUNT 4

typedef struct Move {
  union {
    uint32_t all;
    Step step[STEP_COUNT];
  } steps;
  int step_count;
} Move;
Move new_move();

typedef int32_t Score;
#define INFINITY 0x100000 // 2^20
#define VICTORY 0xfffff   // INFINITY - 1

Square first_square(Bitboard board);
Colour colour_at_square(Position position, Square square);
Type type_at_square(Position position, Colour colour, Square square);
Bitboard bitboard_at(Square square);
Bitboard all_neighbours(Bitboard bitboard);
Bitboard square_neighbours(Square square);

#endif // TYPES_H
