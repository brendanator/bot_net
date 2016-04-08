#include "eval.h"
#include "utils.h"

#include <stdlib.h>
#include <math.h>

int manhattan_distances[SQUARE_COUNT][SQUARE_COUNT];

int manhattan_distance(Square a, Square b) {
  return abs(a/8 - b/8) + abs(a%8 - b%8);
}

void init_eval() {
  for (int i = 0; i < SQUARE_COUNT; i++) {
    for (int j = 0; j < SQUARE_COUNT; j++) {
      manhattan_distances[i][j] = manhattan_distance(i, j);
    }
  }
}

Score harlog(Position position) {
  #define Q 1.447530126
  #define G 0.6314442034
  #define SCALE 0.000125070098913

  int gold_rabbits = population(position.pieces[GOLD][RABBIT]);
  int silver_rabbits = population(position.pieces[SILVER][RABBIT]);
  int gold_pieces = population(position.pieces[GOLD][ALL]);
  int silver_pieces = population(position.pieces[SILVER][ALL]);
  int gold_stronger = gold_pieces - gold_rabbits;
  int silver_stronger = silver_pieces - silver_rabbits;

  double score = 0;

  for (Type type = CAT; type <= ELEPHANT; type++) {
    int silver_type = population(position.pieces[SILVER][type]);
    int gold_type = population(position.pieces[GOLD][type]);
    gold_stronger -= gold_type;
    silver_stronger -= silver_type;

    if (silver_stronger) {
      score += gold_type / (Q + silver_stronger);
    } else {
      score += 2 * gold_type / Q;
    }

    if (gold_stronger) {
      score -= silver_type / (Q + gold_stronger);
    } else {
      score -= 2 * silver_type / Q;
    }
  }

  score += G * log((double)gold_rabbits * gold_pieces / (silver_rabbits * silver_pieces));

  return score / SCALE;
}

Score piece_alignment(Position position) {
  Score score = 0;
  for (Type type = CAT; type <= ELEPHANT; type++) {
    Bitboard gold_stronger = position.pieces[GOLD][type];
    Bitboard silver_weaker = position.pieces[SILVER][type-1];

    while (gold_stronger) {
      Square strong = first_square(gold_stronger);
      while (silver_weaker) {
        Square weak = first_square(silver_weaker);

        score += type * type * (14 - manhattan_distances[strong][weak]);

        silver_weaker &= ~bitboard_at(weak);
      }
      gold_stronger &= ~bitboard_at(strong);
    }

    Bitboard silver_stronger = position.pieces[SILVER][type];
    Bitboard gold_weaker = position.pieces[GOLD][type-1];

    while (silver_stronger) {
      Square strong = first_square(silver_stronger);
      while (gold_weaker) {
        Square weak = first_square(gold_weaker);

        score -= type * type * (14 - manhattan_distances[strong][weak]);

        gold_weaker &= ~bitboard_at(weak);
      }
      silver_stronger &= ~bitboard_at(strong);
    }
  }
  return score;
}

Score trap_control(Position position) {
  Bitboard trap_neighbours = all_neighbours(TRAPS);
  Score gold_trap_control = population(trap_neighbours & position.pieces[GOLD][ALL]);
  Score silver_trap_control = population(trap_neighbours & position.pieces[SILVER][ALL]);
  return gold_trap_control - silver_trap_control;
}

Score winner(Position position) {
  return position.winner * (VICTORY - (position.turn * 4) - position.steps);
}

Score gaussian_noise(double standard_deviation) {
  static double spare;
  static bool has_spare;

  if (has_spare) {
    has_spare = false;
    return spare * standard_deviation;
  } else {
    double s, x, y;
    do {
      x = 2 * ((double)rand() / RAND_MAX) - 1;
      y = 2 * ((double)rand() / RAND_MAX) - 1;
      s = x*x + y*y;
    } while (s >= 1 || s == 0);

    s = sqrt(-2 * log(s) / s);
    spare = y * s;
    has_spare = true;
    return x * s * standard_deviation;
  }
}

Score eval(Position position) {
  Score score = winner(position);

  if (!score) {
    score = 
      harlog(position) +
      trap_control(position) +
      piece_alignment(position);
      // gaussian_noise(10);
  }

  score = min(score, VICTORY);
  score = max(score, -VICTORY);

  if (turn_colour(position.turn) == GOLD) {
    return score;
  } else {
    return -score;
  }
}

