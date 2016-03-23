#include "minunit.h"
#include "../src/aei.h"

char * test_bit_at_and_first_square() {
  for (int i = -1; i < 64; ++i) {
    mu_assert(first_square(bit_at(i)) == i);
  }
  return 0;
}

char * test_generate_single_step() {
  init_move_generation();

  position first;
  position second;
  new_game(&first);
  char *str_steps[] = { "Ra1", "eh8", "Hc2", "dd2" };
  makemove(&first, &second, str_steps, 4);

  move moves[128];
  int count = generate_single_steps(&second, moves);

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

int main() {
  mu_init();
  mu_run_test(test_bit_at_and_first_square);
  mu_run_test(test_generate_single_step);
  mu_report();
}
