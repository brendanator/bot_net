#include "aei.h"
#include "transposition.h"
#include "move_generation.h"
#include "eval.h"

int main(void) {
  init_transposition_table();
  init_move_generation();
  init_eval();

  aei_loop();
}
