#include <stdlib.h>
#include <string.h>

#include "minunit.h"
#include "../src/aei.h"

char * test_parse_message() {
  char *input = (char *)malloc(32 * sizeof(char));;
  char *message = (char *)malloc(32 * sizeof(char));;
  char *options[64];
  strcpy(input, "makemove Ra2e Rh2w\n");

  int count = parse_message(input, message, options);

  mu_assert(count == 2);
  mu_assert(strcmp(message, "makemove") == 0);
  mu_assert(strcmp(options[0], "Ra2e") == 0);  
  mu_assert(strcmp(options[1], "Rh2w") == 0);  
  return 0;
}

char * test_parse_steps() {
  step steps[4];
  char *str_steps[] = { "Ra1e", "eh8s", "Hc2", "df6x" };

  parse_steps(steps, str_steps, 4);

  mu_assert(steps[0].from == 0);
  mu_assert(steps[0].to == 1);
  mu_assert(steps[0].colour == GOLD);
  mu_assert(steps[0].type == RABBIT);

  mu_assert(steps[1].from == 63);
  mu_assert(steps[1].to == 55);
  mu_assert(steps[1].colour == SILVER);
  mu_assert(steps[1].type == ELEPHANT);

  mu_assert(steps[2].from == -1);
  mu_assert(steps[2].to == 10);
  mu_assert(steps[2].colour == GOLD);
  mu_assert(steps[2].type == HORSE);

  mu_assert(steps[3].from == 45);
  mu_assert(steps[3].to == -1);
  mu_assert(steps[3].colour == SILVER);
  mu_assert(steps[3].type == DOG);

  return 0;
}

char * test_makemove() {
  position first;
  position second;
  position third;
  new_game(&first);

  char *str_steps[] = { "Ra1", "eh8", "Hc2" };
  makemove(&first, &second, str_steps, 3);

  mu_assert(second.turn == 1);
  mu_assert(second.pieces[GOLD][RABBIT] == bit_at(0));
  mu_assert(second.pieces[GOLD][HORSE] == bit_at(10));
  mu_assert(second.pieces[SILVER][ELEPHANT] == bit_at(63));

  char *str_steps2[] = { "Ra1e", "eh8s", "Hc2n", "Hc3x" };
  makemove(&second, &third, str_steps2, 3);

  mu_assert(third.pieces[GOLD][RABBIT] == bit_at(1));
  mu_assert(third.pieces[GOLD][HORSE] == EMPTY);
  mu_assert(third.pieces[SILVER][ELEPHANT] == bit_at(55));

  return 0;
}

int main() {
  mu_init();
  mu_run_test(test_parse_message);
  mu_run_test(test_parse_steps);
  mu_run_test(test_makemove);
  mu_report();
}
