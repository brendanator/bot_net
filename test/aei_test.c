#include <stdlib.h>
#include <string.h>

#include "minunit.h"
#include "../src/aei.h"

char * test_parse_message() {
  char input[32];
  char message[32];
  char *options[64];
  strcpy(input, "makemove Ra2e Rh2w\n");

  int count = parse_message(input, message, options);

  mu_assert(count == 2);
  mu_assert(strcmp(message, "makemove") == 0);
  mu_assert(strcmp(options[0], "Ra2e") == 0);  
  mu_assert(strcmp(options[1], "Rh2w") == 0);  
  return 0;
}

char * test_makemove() {
  Position position = new_game();

  char *step_str[] = { "Ra1", "eh8", "Hc2" };
  makemove(&position, step_str, 3);

  mu_assert(position.turn == 1);
  mu_assert(position.pieces[GOLD][RABBIT] == bitboard_at(0));
  mu_assert(position.pieces[GOLD][HORSE] == bitboard_at(10));
  mu_assert(position.pieces[SILVER][ELEPHANT] == bitboard_at(63));

  char *step_str2[] = { "Ra1e", "eh8s", "Hc2n", "Hc3x" };
  makemove(&position, step_str2, 4);

  mu_assert(position.pieces[GOLD][RABBIT] == bitboard_at(1));
  mu_assert(position.pieces[GOLD][HORSE] == EMPTY);
  mu_assert(position.pieces[SILVER][ELEPHANT] == bitboard_at(55));

  return 0;
}

int main() {
  mu_init();
  mu_run_test(test_parse_message);
  mu_run_test(test_makemove);
  mu_report();
}
