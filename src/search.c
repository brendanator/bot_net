#include "search.h"
#include "move_generation.h"
#include "eval.h"
#include "transposition.h"

#include <stdlib.h>

Score negamax(Position position, Score alpha, Score beta, int depth) {
  Score alphaOriginal = alpha;

  Transposition transposition;
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
  }

  if (depth <= 0 || position.gameover) {
    return eval(position);
  }

  Move moves[32000];
  int count = generate_moves(position, new_move(), moves, 0);

  Score best_score = -INFINITY;
  Move best_move;
  for (int i = 0; i < count; i++) {
    Position next = position;
    make_move(&next, moves[i]);

    Score score = -negamax(next, -beta, -alpha, depth-1);

    if (score > alpha) {
      alpha = score;
    }
    if (score > best_score) {
      best_score = score;
      best_move = moves[i];
    }
    if (alpha >= beta) {
      break;
    }
  }

  transposition.best_move = best_move;
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

typedef struct MoveScore {
  Move move;
  Score score;
  Position position;
} MoveScore;

int compareMoveScores(const void *a, const void *b) {
  // Sort higher scores ahead of lower scores
  return ((MoveScore*) b)->score - ((MoveScore*) a)->score;
}

Move find_best_move(Position position) {
  Move *moves = malloc(32000 * sizeof(Move));
  int count = generate_moves(position, new_move(), moves, 0);

  MoveScore move_scores[count];
  for (int i = 0; i < count; i++) {
    Position next = position;
    make_move(&next, moves[i]);
    Score score = eval(next);
    MoveScore move_score = { .move = moves[i], .score = score, .position = next };
    move_scores[i] = move_score;
  }

  qsort(move_scores, count, sizeof(MoveScore), compareMoveScores);

  Move best_move;
  Score best_score = -INFINITY;
  for (int i = 0; i < count; i++) {
    Score score = negamax(move_scores[i].position, -INFINITY, INFINITY, 1);

    if (score > best_score) {
      best_score = score;
      best_move = moves[i];
    }
  }

  free(moves);

  return best_move;
}
