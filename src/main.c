#include "aei.h"
#include "transposition.h"
#include "move_generation.h"

int main(void) {
  init_transposition_table();
  init_move_generation();

  aei_loop();
}
