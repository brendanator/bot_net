#include "print.h"
#include "move_generation.h"

#include <stdio.h>

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

void print_short_position(Position position) {
  printf("%d%c [", 1+position.turn/2, "gs"[position.turn%2]);
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

void print_step(Position position, Step step) {
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
  for (int i = 0; i < step_count(move); ++i) {
    Step step = move.step[i];
    if (step != PASS_STEP) {
      print_step(position, step);
      make_step(&position, step, i);
    }
  }

  printf("\n");
}
