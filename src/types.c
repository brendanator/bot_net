#include "types.h"


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

int step_count(Move move) {
  for (int i = 0; i < STEP_COUNT; ++i) {
    if (move.step[i] == EMPTY_STEP) {
      return i;
    }
  }
  return STEP_COUNT;
}
