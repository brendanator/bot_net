#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "move_generation.h"

Bitboard bitboard_at(Square square) {
  if (square >= 0) {
    return 1ULL << square;
  } else {
    return EMPTY;
  }
}

Bitboard piece_steps[2][7][64];
void init_piece_steps() {
  for (Colour colour = GOLD; colour <= SILVER; colour++) {
    for (Type type = ALL; type <= ELEPHANT; type++) {
      for (Square square = 0; square < 64; square++) {
        Bitboard steps = EMPTY;

        if (type != RABBIT || colour == GOLD) {
          steps |= (bitboard_at(square) & NOT_8_RANK) << 8; // north
        }

        if (type != RABBIT || colour == SILVER) {
          steps |= (bitboard_at(square) & NOT_1_RANK) >> 8; // south
        }

        steps |= (bitboard_at(square) & NOT_H_FILE) << 1;   // east
        steps |= (bitboard_at(square) & NOT_A_FILE) >> 1;   // west

        piece_steps[colour][type][square] = steps;
      }
    }
  }
}

Position new_game() {
  Position position;
  for (Type type = ALL; type <= ELEPHANT; type++) {
    position.pieces[GOLD][type] = EMPTY;
    position.pieces[SILVER][type] = EMPTY;
  }

  position.turn = 0;

  return position;
}

Bitboard all_neighbours(Bitboard board) {
  return
    (board & NOT_8_RANK) << 8 | // north
    (board & NOT_1_RANK) >> 8 | // south
    (board & NOT_H_FILE) << 1 | // east
    (board & NOT_A_FILE) >> 1;  // west
}

void init_square_neighbours() {
  for (Square square = 0; square < 64; square++) {
    square_neighbours[square] = all_neighbours(bitboard_at(square));
  }
}

Square first_square(Bitboard board) {
  return __builtin_ffsl(board) - 1;
}

Type type_at_square(Position position, Colour colour, Square square) {
  Bitboard target = bitboard_at(square);

  for (Type type = RABBIT; type <= ELEPHANT; type++) {
    if (position.pieces[colour][type] & target) {
      return type;
    }
  }

  return -1;
}

void make_step(Position *position, Step step) {
  position->pieces[step.colour][step.type] &= ~bitboard_at(step.from);
  position->pieces[step.colour][step.type] |= bitboard_at(step.to);
  position->pieces[step.colour][ALL] &= ~bitboard_at(step.from);
  position->pieces[step.colour][ALL] |= bitboard_at(step.to);

  Bitboard trap_pieces = position->pieces[step.colour][ALL] & TRAPS;
  while (trap_pieces) {
    Square trap_piece = first_square(trap_pieces);
    Bitboard trap_square = bitboard_at(trap_piece);

    if (!(position->pieces[step.colour][ALL] & square_neighbours[trap_piece])) {
      position->pieces[step.colour][type_at_square(*position, step.colour, trap_piece)] &= ~trap_square;
      position->pieces[step.colour][ALL] &= ~trap_square;
      break;
    }

    trap_pieces &= ~trap_square;
  }
}

void make_move(Position *position, Move move) {
  for (int i = 0; i < move.step_count; i++) {
    make_step(position, move.step[i]);
  }
}

int generate_single_steps(Position position, Move moves[]) {
  int move_count = 0;
  Colour colour = position.turn & 1;
  Colour enemy = colour^1;
  Bitboard empty_squares = ~(position.pieces[GOLD][ALL] | position.pieces[SILVER][ALL]);
  Bitboard stronger = position.pieces[enemy][ALL];

  for (Type type = RABBIT; type <= ELEPHANT; type++) {
    stronger &= ~position.pieces[enemy][type];
    Bitboard unfrozen = all_neighbours(position.pieces[colour][ALL]) | (~all_neighbours(stronger));
    Bitboard pieces = position.pieces[colour][type] & unfrozen;

    while (pieces) {
      Square from = first_square(pieces);
      Bitboard steps = empty_squares & piece_steps[colour][type][from];

      while (steps) {
        Square to = first_square(steps);

        moves[move_count].step[0].from = from;
        moves[move_count].step[0].to = to;
        moves[move_count].step[0].type = type;
        moves[move_count].step[0].colour = colour;
        moves[move_count].step_count = 1;
        move_count++;

        steps &= ~bitboard_at(to);
      }

      pieces &= ~bitboard_at(from);
    }
  }

  return move_count;
}

int generate_push_steps(Position position, Move moves[]) {
  int move_count = 0;
  Colour colour = position.turn & 1;
  Colour enemy = colour^1;
  Bitboard empty_squares = ~(position.pieces[GOLD][ALL] | position.pieces[SILVER][ALL]);
  Bitboard stronger = position.pieces[enemy][ALL] & ~position.pieces[enemy][RABBIT];
  Bitboard weaker = EMPTY;

  for (Type type = CAT; type <= ELEPHANT; type++) {
    stronger &= ~position.pieces[enemy][type];
    weaker |= position.pieces[enemy][type-1];
    Bitboard unfrozen = all_neighbours(position.pieces[colour][ALL]) | (~all_neighbours(stronger));
    Bitboard pushers = position.pieces[colour][type] & all_neighbours(weaker) & unfrozen;

    while (pushers) {
      Square push_from = first_square(pushers);
      Bitboard victims = square_neighbours[push_from] & weaker;

      while (victims) {
        Square victim_from = first_square(victims);
        Type victim_type = type_at_square(position, enemy, victim_from);
        Bitboard victim_steps = empty_squares & piece_steps[enemy][ALL][victim_from];

        while (victim_steps) {
          Square victim_to = first_square(victim_steps);

          moves[move_count].step[0].from = victim_from;
          moves[move_count].step[0].to = victim_to;
          moves[move_count].step[0].type = victim_type;
          moves[move_count].step[0].colour = enemy;

          moves[move_count].step[1].from = push_from;
          moves[move_count].step[1].to = victim_from;
          moves[move_count].step[1].type = type;
          moves[move_count].step[1].colour = colour;

          moves[move_count].step_count = 2;

          move_count++;

          victim_steps &= ~bitboard_at(victim_to);
        }

        victims &= ~bitboard_at(victim_from);
      }

      pushers &= ~bitboard_at(push_from);
    }
  }
  return move_count;
}

int generate_pull_steps(Position position, Move moves[]) {
  int move_count = 0;
  Colour colour = position.turn & 1;
  Colour enemy = colour^1;
  Bitboard empty_squares = ~(position.pieces[GOLD][ALL] | position.pieces[SILVER][ALL]);
  Bitboard stronger = position.pieces[enemy][ALL] & ~position.pieces[enemy][RABBIT];
  Bitboard weaker = EMPTY;

  for (Type type = CAT; type <= ELEPHANT; type++) {
    stronger &= ~position.pieces[enemy][type];
    weaker |= position.pieces[enemy][type-1];
    Bitboard unfrozen = all_neighbours(position.pieces[colour][ALL]) | (~all_neighbours(stronger));
    Bitboard pullers = position.pieces[colour][type] & all_neighbours(weaker) & unfrozen;

    while (pullers) {
      Square pull_from = first_square(pullers);
      Bitboard victims = square_neighbours[pull_from] & weaker;
    
      while (victims) {
        Square victim_from = first_square(victims);
        Type victim_type = type_at_square(position, enemy, victim_from);
        Bitboard pull_steps = empty_squares & piece_steps[enemy][ALL][pull_from];

        while (pull_steps) {
          Square piece_to = first_square(pull_steps);

          moves[move_count].step[0].from = pull_from;
          moves[move_count].step[0].to = piece_to;
          moves[move_count].step[0].type = type;
          moves[move_count].step[0].colour = colour;

          moves[move_count].step[1].from = victim_from;
          moves[move_count].step[1].to = pull_from;
          moves[move_count].step[1].type = victim_type;
          moves[move_count].step[1].colour = enemy;

          moves[move_count].step_count = 2;

          move_count++;

          pull_steps &= ~bitboard_at(piece_to);
        }

        victims &= ~bitboard_at(victim_from);
      }

      pullers &= ~bitboard_at(pull_from);
    }
  }
  return move_count;
}

int generate_moves(Position position, Move moves[]) {
  int count = 0;
  count += generate_single_steps(position, moves);
  count += generate_push_steps(position, &moves[count]);
  count += generate_pull_steps(position, &moves[count]);
  return count;
}

Move find_best_move(Position position) {
  Move best_move;
  Move moves[128];
  int count = generate_moves(position, moves);

  Position next = position;
  int rand_move = rand() % count;
  make_move(&next, moves[rand_move]);
  best_move = moves[rand_move];
  int best_score = -__builtin_popcountl(next.pieces[GOLD][ALL]); // + __builtin_popcountl(next.pieces[SILVER][ALL]);
  for (int i = 0; i < count; i++) {
    Position next;
    make_move(&next, moves[i]);
    int score = -__builtin_popcountl(next.pieces[GOLD][ALL]); // + __builtin_popcountl(next.pieces[SILVER][ALL]);
    if (score > best_score) {
      best_score = score;
      best_move = moves[i];
    }
  }

  return best_move;
}

void print_bitboard(Bitboard bitboard) {
  for (int row = 7; row >= 0; row--) {
    for (int col = 0; col < 8; col--) {
      Square square = row * 8 + col;
      if (bitboard & bitboard_at(square)) {
        printf("x ");
      } else {
        printf("  ");
      }
    }
    printf("\n");
  }
}

void print_position(Position position) {
  printf("%d%c\n", 1+position.turn/2, "gs"[position.turn%2]);
  printf(" +-----------------+\n");
  for (int row = 7; row >= 0; row--) {
    printf("%d| ", row+1);
    for (int col = 0; col < 8; col++) {
      Square square = row * 8 + col;

      if (position.pieces[GOLD][ALL] & bitboard_at(square)) {
        Type type = type_at_square(position, GOLD, square);
        printf("%c", "-RCDHME-"[type]);
      } else if (position.pieces[SILVER][ALL] & bitboard_at(square)) {
        Type type = type_at_square(position, SILVER, square);
        printf("%c", "-rcdhme-"[type]);
      } else if (TRAPS & bitboard_at(square)) {
        printf("x");
      } else {
        printf(".");
      }

      printf(" ");
    }
    printf("|\n");
  }
  printf(" +-----------------+\n   a b c d e f g h\n");
}

void init_move_generation() {
  init_piece_steps();
  init_square_neighbours();
}
