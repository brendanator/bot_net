#include "search.h"
#include "move_generation.h"
#include "eval.h"

#include <stdlib.h>

Score negamax(Position position, Score alpha, Score beta, int depth) {
  if (depth <= 0) return eval(position);

  Move *moves = malloc(32000 * sizeof(Move));
  Move move = new_move();
  int count = generate_moves(position, move, moves, 0);

  Score best_score = -INFINITY;

  for (int i = 0; i < count; i++) {
    Position next = position;

    make_move(&next, moves[i]);

    Score score = -negamax(next, -beta, -alpha, depth-1);
    if (score > alpha) {
      alpha = score;
      best_score = score;
    } else if (score > best_score) {
      best_score = score;
    }

    if (alpha >= beta) {
      break;
    }
  }

  return best_score;
}

Move find_best_move(Position position) {
  Move *moves = malloc(32000 * sizeof(Move));
  Move move = new_move();

  int count = generate_moves(position, move, moves, 0);

  Move best_move;
  Score best_score = -INFINITY;
  for (int i = 0; i < count; i++) {
    Position next = position;

    make_move(&next, moves[i]);

    Score score = eval(next);
    if (score > best_score) {
      best_score = score;
      best_move = moves[i];
    }
  }

  free(moves);

  return best_move;
}
