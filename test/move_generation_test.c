#include "minunit.h"

#include "../src/aei.h"
#include "../src/move_generation.h"

char * test_bitboard_at_and_first_square() {
  for (Square square = 0; square < SQUARE_COUNT; square++) {
    mu_assert(first_square(bitboard_at(square)) == square);
  }
  return 0;
}

char * test_generate_single_steps() {
  Position position = new_game();
  char *step_str[] = { "Ra1", "eh8", "Hc2", "dd2" };
  makemove(&position, step_str, 4);

  Move current_move = new_move();
  Move moves[128];
  int count = generate_single_steps(position, current_move, moves);

  mu_assert(count == 2);

  mu_assert(step_from(moves[0].step[0]) == 63);
  mu_assert(step_to(moves[0].step[0]) == 55);

  mu_assert(step_from(moves[1].step[0]) == 63);
  mu_assert(step_to(moves[1].step[0]) == 62);

  return 0;
}

char * test_generate_push_pull_steps() {
  Position position = new_game();
  char *step_str[] = { "Mf5", "ef4", "re5", "rg4", "Cb2", "dc2", "Hc1", "Hb7", "hb8" };
  makemove(&position, step_str, 9);

  Move current_move = new_move();
  Move moves[128];
  int count = generate_push_pull_steps(position, current_move, moves);

  mu_assert(count == 4);

  // 2 pushes
  mu_assert(step_from(moves[0].step[0]) == 37);
  mu_assert(step_to(moves[0].step[0]) == 38);
  mu_assert(step_from(moves[0].step[1]) == 29);
  mu_assert(step_to(moves[0].step[1]) == 37);

  mu_assert(step_from(moves[1].step[0]) == 37);
  mu_assert(step_to(moves[1].step[0]) == 45);
  mu_assert(step_from(moves[1].step[1]) == 29);
  mu_assert(step_to(moves[1].step[1]) == 37);

  // 2 pulls
  mu_assert(step_from(moves[2].step[0]) == 29);
  mu_assert(step_to(moves[2].step[0]) == 21);
  mu_assert(step_from(moves[2].step[1]) == 37);
  mu_assert(step_to(moves[2].step[1]) == 29);

  mu_assert(step_from(moves[3].step[0]) == 29);
  mu_assert(step_to(moves[3].step[0]) == 28);
  mu_assert(step_from(moves[3].step[1]) == 37);
  mu_assert(step_to(moves[3].step[1]) == 29);

  return 0;
}

int main() {
  init_move_generation();

  mu_init();
  mu_run_test(test_bitboard_at_and_first_square);
  mu_run_test(test_generate_single_steps);
  mu_run_test(test_generate_push_pull_steps);
  mu_report();
}
