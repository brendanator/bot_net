#include "minunit.h"
#include "../src/aei.h"

char * test_bitboard_at_and_first_square() {
  for (Square square = -1; square < 64; square++) {
    mu_assert(first_square(bitboard_at(square)) == square);
  }
  return 0;
}

char * test_generate_single_steps() {
  Position position = new_game();
  char *step_str[] = { "Ra1", "eh8", "Hc2", "dd2" };
  makemove(&position, step_str, 4);

  Move moves[128];
  int count = generate_single_steps(position, moves);

  mu_assert(count == 2);

  mu_assert(moves[0].step[0].from == 63);
  mu_assert(moves[0].step[0].to == 55);
  mu_assert(moves[0].step[0].colour == SILVER);
  mu_assert(moves[0].step[0].type == ELEPHANT);

  mu_assert(moves[1].step[0].from == 63);
  mu_assert(moves[1].step[0].to == 62);
  mu_assert(moves[1].step[0].colour == SILVER);
  mu_assert(moves[1].step[0].type == ELEPHANT);

  return 0;
}

char * test_generate_push_steps() {
  Position position = new_game();
  char *step_str[] = { "Mf5", "ef4", "re5", "Cb2", "dc2", "Hc1", "Hb7", "hb8" };
  makemove(&position, step_str, 8);

  Move moves[128];
  int count = generate_push_steps(position, moves);

  mu_assert(count == 2);

  mu_assert(moves[0].step[0].from == 37);
  mu_assert(moves[0].step[0].to == 38);
  mu_assert(moves[0].step[0].colour == GOLD);
  mu_assert(moves[0].step[0].type == CAMEL);

  mu_assert(moves[0].step[1].from == 29);
  mu_assert(moves[0].step[1].to == 37);
  mu_assert(moves[0].step[1].colour == SILVER);
  mu_assert(moves[0].step[1].type == ELEPHANT);

  mu_assert(moves[1].step[0].from == 37);
  mu_assert(moves[1].step[0].to == 45);
  mu_assert(moves[1].step[0].colour == GOLD);
  mu_assert(moves[1].step[0].type == CAMEL);

  mu_assert(moves[1].step[1].from == 29);
  mu_assert(moves[1].step[1].to == 37);
  mu_assert(moves[1].step[1].colour == SILVER);
  mu_assert(moves[1].step[1].type == ELEPHANT);

  return 0;
}

char * test_generate_pull_steps() {
  Position position = new_game();
  char *step_str[] = { "Mf5", "ef4", "re4", "Cb2", "dc2", "Hc1", "Hb7", "hb8" };
  makemove(&position, step_str, 8);

  Move moves[128];
  int count = generate_pull_steps(position, moves);

  mu_assert(count == 2);

  mu_assert(moves[0].step[0].from == 29);
  mu_assert(moves[0].step[0].to == 21);
  mu_assert(moves[0].step[0].colour == SILVER);
  mu_assert(moves[0].step[0].type == ELEPHANT);

  mu_assert(moves[0].step[1].from == 37);
  mu_assert(moves[0].step[1].to == 29);
  mu_assert(moves[0].step[1].colour == GOLD);
  mu_assert(moves[0].step[1].type == CAMEL);

  mu_assert(moves[1].step[0].from == 29);
  mu_assert(moves[1].step[0].to == 30);
  mu_assert(moves[1].step[0].colour == SILVER);
  mu_assert(moves[1].step[0].type == ELEPHANT);

  mu_assert(moves[1].step[1].from == 37);
  mu_assert(moves[1].step[1].to == 29);
  mu_assert(moves[1].step[1].colour == GOLD);
  mu_assert(moves[1].step[1].type == CAMEL);

  return 0;
}

int main() {
  init_move_generation();

  mu_init();
  mu_run_test(test_bitboard_at_and_first_square);
  mu_run_test(test_generate_single_steps);
  mu_run_test(test_generate_push_steps);
  mu_run_test(test_generate_pull_steps);
  mu_report();
}
