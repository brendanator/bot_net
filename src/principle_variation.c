#include "principle_variation.h"
#include "transposition.h"
#include "move_generation.h"

#include <string.h>

#define MAX_PV_LENGTH 40

Step pv_table[MAX_PV_LENGTH][MAX_PV_LENGTH];

void clear_principle_variation() {
  memset(&pv_table, 0, sizeof(pv_table));
}

void update_principle_variation(Move move, int start_ply) {
  int plies_to_copy = move.step_count - (start_ply % STEP_COUNT);
  for (int ply = start_ply + plies_to_copy - 1; ply >= start_ply; ply--) {
    pv_table[ply][0] = move.steps.step[ply % STEP_COUNT];
    memcpy(&pv_table[ply][1], &pv_table[ply + 1][0], (MAX_PV_LENGTH - ply - 1) * sizeof(Step));
  }
}

void restore_principle_variation_to_transposition_table(Position position) {
  PrincipleVariation pv = principle_variation();
  for (int move_number = 0; move_number < pv.move_count; move_number++) {
    Move move = pv.move[move_number];
    Transposition transposition = { .best_move = move.steps.all };
    for (int step_number = 0; step_number < move.step_count; step_number++) {
      save_transposition(position, transposition);
      make_step(&position, move.steps.step[step_number], step_number);
    }
  }
}

PrincipleVariation principle_variation() {
  PrincipleVariation pv = {};

  Step *pv_line = pv_table[0];
  int ply = 0;
  Step step = pv_line[ply];
  while (step && ply < MAX_PV_LENGTH) {
    int step_number = ply % STEP_COUNT;
    if (step_number == 0) {
      pv.move_count++;
    }

    pv.move[pv.move_count-1].steps.step[step_number] = step;
    pv.move[pv.move_count-1].step_count++;

    ply++;
    step = pv_line[ply];
  }

  return pv;  
}
