#include "minunit.h"

#include "../src/aei.h"
#include "../src/eval.h"

#include <stdlib.h>

char * test_manhattan_distance() {
  mu_assert(manhattan_distance(0, 0) == 0);
  mu_assert(manhattan_distance(0, 1) == 1);
  mu_assert(manhattan_distance(0, 4) == 4);
  mu_assert(manhattan_distance(0, 14) == 7);
  mu_assert(manhattan_distance(0, 63) == 14);
  mu_assert(manhattan_distance(10, 18) == 1);
  mu_assert(manhattan_distance(10, 47) == 9);
  mu_assert(manhattan_distance(10, 48) == 7);

  mu_assert(manhattan_distance(0, 0) == 0);
  mu_assert(manhattan_distance(1, 0) == 1);
  mu_assert(manhattan_distance(4, 0) == 4);
  mu_assert(manhattan_distance(14, 0) == 7);
  mu_assert(manhattan_distance(63, 0) == 14);
  mu_assert(manhattan_distance(18, 10) == 1);
  mu_assert(manhattan_distance(47, 10) == 9);
  mu_assert(manhattan_distance(48, 10) == 7);

  return 0;
}

char * test_harlog() {
  Position position = new_game();

  // Dog for 2 rabbits is equal
  char *step_str[] = { "Ra1", "Rb1", "Rc1", "Rd1", "Re1", "Rf1", "Rg1", "Rh1", "Ha2", "Db2", "Cc2", "Md2", "Ee2", "Cf2", "Hh2", "ra8", "rb8", "rc8", "rd8", "re8", "rf8", "cc7", "cf7", "da7", "dh7", "hb7", "hg7", "me7", "ed7" };
  makemove(&position, step_str, 29);
  mu_assert(abs(harlog(position)) == 0);

  // First rabbit is worth 1000 milli-rabbits
  position = new_game();
  char *step_str2[] = { "Ra1", "Rb1", "Rc1", "Rd1", "Re1", "Rf1", "Rg1", "Rh1", "Ha2", "Db2", "Cc2", "Md2", "Ee2", "Cf2", "Dg2", "Hh2", "ra8", "rb8", "rc8", "rd8", "re8", "rf8", "rg8", "cc7", "cf7", "da7", "dh7", "hb7", "hg7", "me7", "ed7" };
  makemove(&position, step_str2, 31);
  mu_assert(abs(harlog(position)) == 1000);

  return 0;
}

int main() {
  init_eval();

  mu_init();
  mu_run_test(test_manhattan_distance);
  mu_run_test(test_harlog);
  mu_report();
}
