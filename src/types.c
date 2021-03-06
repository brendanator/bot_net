#include "types.h"
#include "print.h"
#include "utils.h"

#include <stdlib.h>

Colour turn_colour(int turn) {
  return turn % 2;
}

Colour enemy_colour(Colour colour) {
  return colour ^ 1;
}

Bitboard north(Bitboard bitboard) {
  return (bitboard & NOT_8_RANK) << 8;
}

Bitboard south(Bitboard bitboard) {
  return (bitboard & NOT_1_RANK) >> 8;
}

Bitboard east(Bitboard bitboard) {
  return (bitboard & NOT_H_FILE) << 1;
}

Bitboard west(Bitboard bitboard) {
  return (bitboard & NOT_A_FILE) >> 1;
}

int population(Bitboard bitboard) {
  return __builtin_popcountl(bitboard);
}

Position new_game() {
  Position position = {};
  return position;
}

Move new_move() {
  Move move = {};
  return move;
}

Step new_step(Square from, Square to) {
  Direction direction;
  switch (to - from) {
    case  8: direction = NORTH; break;
    case -8: direction = SOUTH; break;
    case  1: direction = EAST; break;
    case -1: direction = WEST; break;
  }
  return (from << 2) + direction;
}

Square step_from(Step step) {
  return step >> 2;
}

Square step_to(Step step) {
  Square from = step_from(step);
  switch (step & 3) {
    case NORTH: return from + 8;
    case SOUTH: return from - 8;
    case EAST:  return from + 1;
    case WEST:  return from - 1;
    default:    return from;
  }
}

Direction step_direction(Step step) {
  return step & 3;
}

Bitboard bitboard_at(Square square) {
  return 1ULL << square;
}

Bitboard all_neighbours(Bitboard board) {
  return north(board) | south(board) | east(board) | west(board);
}

Bitboard square_neighbours(Square square) {
  return all_neighbours(bitboard_at(square));
}

Square first_square(Bitboard board) {
  return __builtin_ffsl(board) - 1;
}

Type type_at_square(Position position, Colour colour, Square square) {
  Bitboard target = bitboard_at(square);

  for (Type type = RABBIT; type <= ELEPHANT; type++) {
    if (position.pieces[colour][type] & target) {
      return type;
    }
  }

  assert(false);
  return -1;
}

Colour colour_at_square(Position position, Square square) {
  Bitboard target = bitboard_at(square);

  for (Colour colour = GOLD; colour <= SILVER; colour++) {
    if (position.pieces[colour][ALL] & target) {
      return colour;
    }
  }

  assert(false);
  return -1;
}

