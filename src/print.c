#include "print.h"
#include "move_generation.h"

#include <execinfo.h>

void print_bitboard(Bitboard bitboard) {
  printf("+-----------------+\n");
  for (int row = 7; row >= 0; row--) {
    printf("| ");
    for (int col = 0; col < 8; col++) {
      Square square = row * 8 + col;
      if (bitboard & bitboard_at(square)) {
        printf("x ");
      } else {
        printf("  ");
      }
    }
    printf("|\n");
  }
  printf("+-----------------+\n");
}

void print_turn(Position position) {
  printf("%d%c", 1+position.turn/2, "gs"[position.turn%2]);
}

void print_position(Position position) {
  print_turn(position);
  printf("\n +-----------------+\n");

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

void print_short_position(Position position) {
  print_turn(position);
  printf(" [");

  for (int row = 7; row >= 0; row--) {
    for (int col = 0; col < 8; col++) {
      Square square = row * 8 + col;

      if (position.pieces[GOLD][ALL] & bitboard_at(square)) {
        Type type = type_at_square(position, GOLD, square);
        printf("%c", "-RCDHME-"[type]);
      } else if (position.pieces[SILVER][ALL] & bitboard_at(square)) {
        Type type = type_at_square(position, SILVER, square);
        printf("%c", "-rcdhme-"[type]);
      } else {
        printf(" ");
      }
    }
  }
  printf("]");
}

char piece_char(Colour colour, Type type) {
  return "-RCDHME--rcdhme-"[ colour * 8 + type ];
}

void print_step(Step step) {
  Square from = step_from(step);

  char row = 'a' + (from % 8);
  char col = '1' + (from / 8);
  char direction;
  switch (step_direction(step)) {
    case NORTH: direction = 'n'; break;
    case SOUTH: direction = 's'; break;
    case EAST: direction = 'e'; break;
    case WEST: direction = 'w'; break;
  }

  printf("%c%c%c ", row, col, direction);
}

void print_steps(Move move) {
  for (int step_number = 0; step_number < move.step_count; step_number++) {
    print_step(move.steps.step[step_number]);
  }
  printf("\n");
}

void print_full_step(Position position, Step step) {
  Square from = step_from(step);
  Square to = step_to(step);
  Colour colour = colour_at_square(position, from);
  Type type = type_at_square(position, colour, from);

  char piece = piece_char(colour, type);
  char row = 'a' + (from % 8);
  char col = '1' + (from / 8);
  char direction;
  switch (step_direction(step)) {
    case NORTH: direction = 'n'; break;
    case SOUTH: direction = 's'; break;
    case EAST: direction = 'e'; break;
    case WEST: direction = 'w'; break;
  }

  printf("%c%c%c%c ", piece, row, col, direction);

  position.pieces[colour][type] &= ~bitboard_at(from);
  position.pieces[colour][type] |= bitboard_at(to);
  position.pieces[colour][ALL] &= ~bitboard_at(from);
  position.pieces[colour][ALL] |= bitboard_at(to);

  Bitboard trap = square_neighbours(from) & TRAPS;
  if (trap) {
    Square trap_square = first_square(trap);

    if (position.pieces[colour][ALL] & trap && !(position.pieces[colour][ALL] & square_neighbours(trap_square))) {
      piece = piece_char(colour, type_at_square(position, colour, trap_square));
      row = 'a' + (trap_square % 8);
      col = '1' + (trap_square / 8);
      printf("%c%c%cx ", piece, row, col);
    }
  }
}

void print_move(Position position, Move move) {
  for (int i = 0; i < move.step_count; ++i) {
    Step step = move.steps.step[i];
    if (step != PASS_STEP && step != EMPTY_STEP) {
      print_full_step(position, step);
      make_step(&position, step, i);
    }
  }

  printf("\n");
}

void print_principle_variation(Position position, PrincipleVariation pv) {
  printf("info pv ");

  for (int move_number = 0; move_number < pv.move_count; move_number++) {
    print_turn(position);
    printf(" ");

    Move move = pv.move[move_number];
    for (int step_number = 0; step_number < move.step_count; step_number++) {
      Step step = move.steps.step[step_number];
      if (step != PASS_STEP) {
        print_full_step(position, step);
      }
      make_step(&position, step, step_number);
    }
  }

  printf("\n");
}

void print_transposition(Transposition transposition) {
  printf("Transposition - ");
  Move move = best_move(transposition);
  for (int i = 0; i < move.step_count; ++i) {
    print_step(move.steps.step[i]);
  }

  char *bound;
  switch (transposition.bound) {
    case EXACT: bound = "Exact score"; break;
    case LOWER_BOUND: bound = "Lower bound"; break;
    case UPPER_BOUND: bound = "Upper bound"; break;
    case NONE: bound = "None"; break;
  }

  printf("- %s = %+.2f at depth %d\n", bound, transposition.score / 1000.0, transposition.depth);
}

void print_stack_trace() {
  void *array[10];
  size_t size = backtrace(array, 10);
  fprintf(stderr, "Stack frames.\n");
  backtrace_symbols_fd(array, size, fileno(stderr));
  fflush(stderr);
}
