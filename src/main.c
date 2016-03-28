#include "aei.h"

int main(void) {
  init_move_generation();
  init_transposition_table();

  aei_loop();
}
