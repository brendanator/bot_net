#include "search.h"
#include "move_generation.h"
#include "eval.h"
#include "transposition.h"
#include "print.h"
#include "utils.h"
#include "killer.h"
#include "time_management.h"

#include <stdlib.h>
#include <time.h>

typedef struct Stats {
  long nodes_searched;
  long transposition_hit;
  long alpha_raised;
  long beta_cutoff;
} Stats;

Stats stats = {};

void clear_stats() {
  Stats new_stats = {};
  stats = new_stats;
}


bool end_search = false;


Score negamax(Position position, Move current_move, Score alpha, Score beta, int depth, int ply) {
  if ((stats.nodes_searched & 0xffff) == 0 && time_up()) {
    end_search = true;
  }

  Score alphaOriginal = alpha;

  Move moves[128];
  int count = 0;

  Transposition transposition = {};
  if (load_transposition(position, &transposition)) {
    stats.transposition_hit++;

    if (transposition.depth >= depth) {
      if (transposition.bound == EXACT) {
        return transposition.score;
      } else if (transposition.bound == LOWER_BOUND) {
        alpha = max(alpha, transposition.score);
      } else if (transposition.bound == UPPER_BOUND && transposition.score < beta) {
        beta = min(beta, transposition.score);
      }

      if (alpha >= beta) {
        stats.beta_cutoff++;
        return transposition.score;
      }
    }

    if (transposition.best_move) {
      Move move = best_move(transposition);
      if (is_legal(position, move)) {
        moves[count++] = move;
      }
    }
  }

  if (depth <= 0 || position.winner) {
    return eval(position);
  }

  count += add_killers(position, current_move, ply, &moves[count]);

  if (depth > 1 && current_move.step_count < STEP_COUNT-1) {
    count += generate_push_pull_steps(position, current_move, &moves[count]);
  }
  if (current_move.step_count < STEP_COUNT) {
    count += generate_single_steps(position, current_move, &moves[count]);
  }
  if (current_move.step_count > 0 && current_move.step_count < STEP_COUNT) {
    count += generate_pass(current_move, &moves[count]);
  }

  Score best_score = -INFINITY;
  Move best_move = new_move();
  for (int i = 0; i < count && !end_search; i++) {
    stats.nodes_searched++;
    Position next = position;
    Move move = moves[i];
    for (int step_number = current_move.step_count; step_number < move.step_count; step_number++) {
      make_step(&next, move.steps.step[step_number], step_number);
    }

    Score score;
    if (position.turn == next.turn) { // Same colour to move
      score = negamax(next, move, alpha, beta, depth - (move.step_count - current_move.step_count), ply + (move.step_count - current_move.step_count));
    } else {
      // TODO - check for repetitions here?
      score = -negamax(next, new_move(), -beta, -alpha, depth - (move.step_count - current_move.step_count), ply + (move.step_count - current_move.step_count));
    }

    if (score > best_score) {
      best_score = score;
      best_move = move;
    }
    if (score >= beta) {
      stats.beta_cutoff++;
      save_killer(move, ply);
      break;
    }
    if (score > alpha) {
      stats.alpha_raised++;
      alpha = score;
      update_principle_variation(move, ply);
    }
  }

  transposition.best_move = best_move.steps.all;
  transposition.depth = depth;
  transposition.score = best_score;
  if (best_score <= alphaOriginal) {
    transposition.bound = UPPER_BOUND;
  } else if (best_score >= beta) {
    transposition.bound = LOWER_BOUND;
  } else {
    transposition.bound = EXACT;
  }
  save_transposition(position, transposition);

  return best_score;
}

void start_search(Position position) {
  start_move_timer(turn_colour(position.turn));
  clear_principle_variation();
  clear_killers();
  clear_stats();
  end_search = false;
}

Move find_best_move(Position position) {
  start_search(position);

  printf("info currmovenumber ");
  print_turn(position);
  printf("\n");

  for (int depth = 2; depth <= 12 && !end_search; depth++) {
    Score score = negamax(position, new_move(), -INFINITY, INFINITY, depth, 0);

    printf("info depth %d\n", depth);
    printf("info score %+.2f\n", score / 1000.0);
    printf("info nodes %'ld\n", stats.nodes_searched);
    printf("info time %ld\n", time_taken());
    print_principle_variation(position, principle_variation());
    restore_principle_variation_to_transposition_table(position);
  }

  printf("log Searched %'ld nodes, %'ld nodes/s, %'ld tthits, %'ld alpha raised, %'ld beta cutoff\n", stats.nodes_searched, stats.nodes_searched / time_taken(), stats.transposition_hit, stats.alpha_raised, stats.beta_cutoff);

  return principle_variation().move[0];
}
