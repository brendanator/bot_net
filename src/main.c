#include "aei.h"
#include "transposition.h"
#include "move_generation.h"
#include "eval.h"
#include <locale.h>

int main(void) {
  init_transposition_table();
  init_move_generation();
  init_eval();
  // Allow printing of commas in numbers
  setlocale(LC_ALL, "");

  aei_loop();
}
