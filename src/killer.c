#include "killer.h"
#include "move_generation.h"
#include "utils.h"

#define MAX_KILLERS 8

typedef struct Killers {
  int count;
  int next;
  Move killer[MAX_KILLERS];
} Killers;

Killers killers_at_ply[16];

void clear_killers() {
  for (int ply = 0; ply < 16; ply++) {
    Killers killers = {};
    killers_at_ply[ply] = killers;
  }  
}

int add_killers(Position position, Move current_move, int ply, Move moves[]) {
  int count = 0;
  Killers killers = killers_at_ply[ply];
  for (int i = 0; i < killers.count; i++) {
    Move killer = killers.killer[i];
    if (killer.step_count > position.steps && is_legal(position, killer)) {
      for (int step_number = 0; step_number < current_move.step_count; step_number++) {
        killer.steps.step[step_number] = current_move.steps.step[step_number];
      }
      moves[count++] = killer;
    }
  }
  return count;
}

void save_killer(Move move, int ply) {
  Killers killers = killers_at_ply[ply];
  killers.killer[killers.next] = move;
  killers.next = (killers.next+1) % MAX_KILLERS;
  killers.count = min(killers.count+1, MAX_KILLERS);
  killers_at_ply[ply] = killers;
}
