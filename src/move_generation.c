#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "move_generation.h"

bitboard bit_at(int n) {
  if (n >= 0)
    return 1ULL << n;
  else
    return EMPTY;
}

bitboard piece_steps[2][7][64];
void init_piece_steps() {
  for (int colour = GOLD; colour <= SILVER; colour++) {
    for (int type = RABBIT; type <= ELEPHANT; type++) {
      for (int square = 0; square < 64; square++) {
        bitboard steps = EMPTY;

        if (type != RABBIT || colour == GOLD)
          steps |= (bit_at(square) & NOT_8_RANK) << 8; // north

        if (type != RABBIT || colour == SILVER)
          steps |= (bit_at(square) & NOT_1_RANK) >> 8; // south

        steps |= (bit_at(square) & NOT_H_FILE) << 1;   // east
        steps |= (bit_at(square) & NOT_A_FILE) >> 1;   // west

        piece_steps[colour][type][square] = steps;
      }
    }
  }
}

void new_game(position *position) {
  for (int type = ALL; type <= ELEPHANT; type++) {
    position->pieces[GOLD][type] = EMPTY;
    position->pieces[SILVER][type] = EMPTY;
  }

  position->turn = 0;
}

bitboard all_neighbours(bitboard board) {
  return
    (board & NOT_8_RANK) << 8 | // north
    (board & NOT_1_RANK) >> 8 | // south
    (board & NOT_H_FILE) << 1 | // east
    (board & NOT_A_FILE) >> 1;  // west
}

bitboard square_neighbours[64];
void init_square_neighbours() {
  for (int square = 0; square < 64; square++) {
    square_neighbours[square] = all_neighbours(bit_at(square));
  }
}

int first_square(bitboard board) {
  return __builtin_ffsl(board) - 1;
}

int type_at_square(position *position, int colour, int square) {
  bitboard target = bit_at(square);

  for (int type = RABBIT; type <= ELEPHANT; type++) {
    if (position->pieces[colour][type] & target)
      return type;
  }

  return -1;
}

void make_step(position *position, step step) {
  position->pieces[step.colour][step.type] &= ~bit_at(step.from);
  position->pieces[step.colour][step.type] |= bit_at(step.to);
  position->pieces[step.colour][ALL] &= ~bit_at(step.from);
  position->pieces[step.colour][ALL] |= bit_at(step.to);

  bitboard trap_pieces = position->pieces[step.colour][ALL] & TRAPS;
  while (trap_pieces) {
    int trap_piece = first_square(trap_pieces);
    bitboard trap_square = bit_at(trap_piece);

    if (!(position->pieces[step.colour][ALL] & square_neighbours[trap_piece])) {
      position->pieces[step.colour][type_at_square(position, step.colour, trap_piece)] &= ~trap_square;
      position->pieces[step.colour][ALL] &= ~trap_square;
      break;
    }

    trap_pieces &= ~trap_square;
  }
}

void make_move(position *current, position *next, move move) {
  *next = *current;

  for (int i = 0; i < move.step_count; i++) {
    make_step(next, move.step[i]);
  }
}

int generate_single_steps(position *position, move *moves) {
  int move_count = 0;
  int colour = position->turn & 1;
  int enemy = colour^1;
  bitboard empty_squares = ~(position->pieces[GOLD][ALL] | position->pieces[SILVER][ALL]);
  bitboard stronger = position->pieces[enemy][ALL];

  for(int type = 1; type < 7; type++) {
    stronger &= ~position->pieces[enemy][type];
    bitboard unfrozen = all_neighbours(position->pieces[colour][ALL]) | (~all_neighbours(stronger));
    bitboard pieces = position->pieces[colour][type] & unfrozen;

    while (pieces) {
      int from = first_square(pieces);
      bitboard steps = empty_squares & piece_steps[colour][type][from];

      while (steps) {
        int to = first_square(steps);

        moves[move_count].step[0].from = from;
        moves[move_count].step[0].to = to;
        moves[move_count].step[0].type = type;
        moves[move_count].step[0].colour = colour;
        moves[move_count].step_count = 1;
        move_count++;

        steps &= ~bit_at(to);
      }

      pieces &= ~bit_at(from);
    }
  }

  return move_count;
}

void find_best_move(position *position, move *best_move) {
  move moves[128];
  int count = generate_single_steps(position, moves);
  *best_move = moves[rand() % count];
}

void print_bitboard(bitboard bitboard) {
  for (int i = 7; i >= 0; --i) {
    for (int j = 0; j < 8; ++j) {
      int x = i*8+j;
      if (bitboard & bit_at(x)) {
        printf("x ");
      } else {
        printf("  ");
      }
    }
    printf("\n");
  }
}

void print_position(position position) {
  printf("%d%c\n", 1+position.turn/2, "gs"[position.turn%2]);
  printf(" +-----------------+\n");
  for (int i = 7; i >= 0; --i) {
    printf("%d| ", i+1);
    for (int j = 0; j < 8; ++j) {
      int x = i*8+j;
      if (position.pieces[GOLD][ALL] & bit_at(x)) {
        int type = type_at_square(&position, GOLD, x);
        printf("%c", "-RCDHME-"[type]);
      } else if (position.pieces[SILVER][ALL] & bit_at(x)) {
        int type = type_at_square(&position, SILVER, x);
        printf("%c", "-rcdhme-"[type]);
      } else if (x == 18 || x == 21 || x == 42 || x == 45) {
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
