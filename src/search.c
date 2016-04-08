#include "search.h"
#include "move_generation.h"
#include "eval.h"
#include "transposition.h"
#include "print.h"

#include <stdlib.h>
#include <time.h>

static int nodes_searched = 0;

Score negamax(Position position, Move current_move, Score alpha, Score beta, int depth) {
  Score alphaOriginal = alpha;

  Move moves[128];
  int count = 0;

  Transposition transposition = {};
  if (load_transposition(position, &transposition) && transposition.depth >= depth) {
    if (transposition.bound == EXACT) {
      return transposition.score;
    } else if (transposition.bound == LOWER_BOUND && transposition.score > alpha) {
      alpha = transposition.score;
    } else if (transposition.bound == UPPER_BOUND && transposition.score < beta) {
      beta = transposition.score;
    }

    if (alpha >= beta) {
      return transposition.score;
    }

    if (transposition.best_move) {
      moves[count++] = best_move(transposition);
    }
  }

  if (depth <= 0 || position.winner) {
    return eval(position);
  }

  if (current_move.step_count < STEP_COUNT-1) {
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
  for (int i = 0; i < count; i++) {
    nodes_searched++;
    Position next = position;
    Move move = moves[i];
    for (int step_number = current_move.step_count; step_number < move.step_count; step_number++) {
      make_step(&next, move.steps.step[step_number], step_number);
    }

    Score score;
    if (position.turn == next.turn) { // Same colour to move
      score = negamax(next, move, alpha, beta, depth - (move.step_count - current_move.step_count));
    } else {
      // TODO - check for repetitions here?

      score = -negamax(next, new_move(), -beta, -alpha, depth - (move.step_count - current_move.step_count));
    }


    if (score > alpha) {
      alpha = score;
    }
    if (score > best_score) {
      best_score = score;
      best_move = move;
    }
    if (alpha >= beta) {
      break;
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

PrincipleVariation principle_variation(Position position) {
  PrincipleVariation pv = {};
  int step_number = 0;
  Transposition transposition;

  while (load_transposition(position, &transposition)) {
    if (pv.move_count == 0 || step_number == STEP_COUNT) {
      step_number = 0;
      pv.move_count++;
    }

    Move move = best_move(transposition);
    pv.move[pv.move_count-1] = move;

    for (; step_number < move.step_count; step_number++) {
      make_step(&position, move.steps.step[step_number], step_number);
    }
  }

  return pv;
}

Move find_best_move(Position position) {
  nodes_searched = 0;
  time_t start_time = time(NULL);

  for (int depth = 2; depth <= 8; depth++) {
    Score score = negamax(position, new_move(), -INFINITY, INFINITY, depth);

    printf("info score %+.2f\n", score / 1000.0);
    printf("info depth %d\n", depth);
    printf("info nodes %d\n", nodes_searched);
    printf("info time %.0f\n", difftime(time(NULL), start_time));
    print_principle_variation(position, principle_variation(position));
  }

  return principle_variation(position).move[0];
}
