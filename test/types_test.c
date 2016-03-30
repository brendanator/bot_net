#include "minunit.h"

#include "../src/move_generation.h"
#include "../src/transposition.h"
char * test_direction() {
  Bitboard board = north(bitboard_at(0));
  mu_assert(board ^ bitboard_at(0));
  mu_assert(board & bitboard_at(8));
  mu_assert((board & ~bitboard_at(8)) == 0);

  board = south(bitboard_at(34));
  mu_assert(board ^ bitboard_at(34));
  mu_assert(board & bitboard_at(26));
  mu_assert((board & ~bitboard_at(26)) == 0);

  board = east(bitboard_at(62));
  mu_assert(board ^ bitboard_at(62));
  mu_assert(board & bitboard_at(63));
  mu_assert((board & ~bitboard_at(63)) == 0);

  board = west(bitboard_at(63) | bitboard_at(33));
  mu_assert(board ^ bitboard_at(63));
  mu_assert(board ^ bitboard_at(33));
  mu_assert(board & bitboard_at(62));
  mu_assert(board & bitboard_at(32));
  mu_assert((board & ~bitboard_at(62) & ~bitboard_at(32)) == 0);

  return 0;
}

char * test_step() {
  Step step = new_step(0, 8);
  mu_assert(step_from(step) == 0);
  mu_assert(step_to(step) == 8);
  mu_assert(step_direction(step) == NORTH);

  step = new_step(34, 26);
  mu_assert(step_from(step) == 34);
  mu_assert(step_to(step) == 26);
  mu_assert(step_direction(step) == SOUTH);

  step = new_step(63, 62);
  mu_assert(step_from(step) == 63);
  mu_assert(step_to(step) == 62);
  mu_assert(step_direction(step) == WEST);

  step = new_step(62, 63);
  mu_assert(step_from(step) == 62);
  mu_assert(step_to(step) == 63);
  mu_assert(step_direction(step) == EAST);

  return 0;
}

int main() {
  mu_init();
  mu_run_test(test_direction);
  mu_run_test(test_step);
  mu_report();
}
