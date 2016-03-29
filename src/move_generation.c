#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "move_generation.h"

Bitboard bitboard_at(Square square) {
  return 1ULL << square;
}

Bitboard piece_steps[COLOUR_COUNT][TYPE_COUNT][SQUARE_COUNT];
void init_piece_steps() {
  for (Colour colour = 0; colour < COLOUR_COUNT; colour++) {
    for (Type type = 0; type < TYPE_COUNT; type++) {
      for (Square square = 0; square < SQUARE_COUNT; square++) {
        Bitboard steps = EMPTY;
        Bitboard square_board = bitboard_at(square);

        if (type != RABBIT || colour == GOLD) {
          steps |= north(square_board);
        }

        if (type != RABBIT || colour == SILVER) {
          steps |= south(square_board);
        }

        steps |= east(square_board);
        steps |= west(square_board);

        piece_steps[colour][type][square] = steps;
      }
    }
  }
}

Position new_game() {
  Position position = { .turn = 0, .hash = 0ULL };

  for (Type type = ALL; type <= ELEPHANT; type++) {
    position.pieces[GOLD][type] = EMPTY;
    position.pieces[SILVER][type] = EMPTY;
  }

  return position;
}

Move new_move() {
  Move move;

  for (int step_number = 0; step_number < STEP_COUNT; step_number++) {
    move.step[step_number] = EMPTY_STEP;
  }

  return move;
}

Bitboard all_neighbours(Bitboard board) {
  return north(board) | south(board) | east(board) | west(board);
}

void init_square_neighbours() {
  for (Square square = 0; square < SQUARE_COUNT; square++) {
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

Colour colour_at_square(Position position, Square square) {
  Bitboard target = bitboard_at(square);

  for (Colour colour = GOLD; colour <= SILVER; colour++) {
    if (position.pieces[colour][ALL] & target) {
      return colour;
    }
  }

  return -1;
}

void place_piece(Position *position, PlacePiece piece) {
  position->pieces[piece.colour][piece.type] |= bitboard_at(piece.square);
  position->pieces[piece.colour][ALL] |= bitboard_at(piece.square);

  position->hash = place_update_hash(position->hash, piece.colour, piece.type, piece.square);
}

void make_step(Position *position, Step step, int step_number) {
  if (step == PASS_STEP) {
    position->hash = pass_update_hash(position->hash, position->turn >> 2, step_number);
    return;
  }

  Square from = step_from(step);
  Square to = step_to(step);
  Colour colour = colour_at_square(*position, from);
  Type type = type_at_square(*position, colour, from);

  position->pieces[colour][type] &= ~bitboard_at(from);
  position->pieces[colour][type] |= bitboard_at(to);
  position->pieces[colour][ALL] &= ~bitboard_at(from);
  position->pieces[colour][ALL] |= bitboard_at(to);

  position->hash = step_update_hash(position->hash, colour, type, from, to, step_number);

  Bitboard trap_pieces = position->pieces[colour][ALL] & TRAPS;
  while (trap_pieces) {
    Square trap_square = first_square(trap_pieces);
    Bitboard trap = bitboard_at(trap_square);

    if (!(position->pieces[colour][ALL] & square_neighbours[trap_square])) {
      Type type = type_at_square(*position, colour, trap_square);
      position->pieces[colour][type] &= ~trap;
      position->pieces[colour][ALL] &= ~trap;

      position->hash = capture_update_hash(position->hash, colour, type, trap_square);

      break;
    }

    trap_pieces &= ~trap;
  }
}

void make_move(Position *position, Move move) {
  int count = step_count(move);
  for (int step_number = 0; step_number < count; step_number++) {
    make_step(position, move.step[step_number], step_number);
  }
}

int generate_single_steps(Position position, Move current_move, Move moves[]) {
  int move_count = 0;
  int step_number = step_count(current_move);
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

        Move move = current_move;
        move.step[step_number] = new_step(from, to);
        moves[move_count++] = move;

        steps &= ~bitboard_at(to);
      }

      pieces &= ~bitboard_at(from);
    }
  }

  return move_count;
}

int generate_push_steps(Position position, Move current_move, Move moves[]) {
  int move_count = 0;
  int step_number = step_count(current_move);
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
        Bitboard victim_steps = empty_squares & piece_steps[enemy][ALL][victim_from];

        while (victim_steps) {
          Square victim_to = first_square(victim_steps);

          Move move = current_move;
          move.step[step_number] = new_step(victim_from, victim_to);
          move.step[step_number+1] = new_step(push_from, victim_from);
          moves[move_count++] = move;

          victim_steps &= ~bitboard_at(victim_to);
        }

        victims &= ~bitboard_at(victim_from);
      }

      pushers &= ~bitboard_at(push_from);
    }
  }
  return move_count;
}

int generate_pull_steps(Position position, Move current_move, Move moves[]) {
  int move_count = 0;
  int step_number = step_count(current_move);
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
        Bitboard pull_steps = empty_squares & piece_steps[enemy][ALL][pull_from];

        while (pull_steps) {
          Square piece_to = first_square(pull_steps);

          Move move = current_move;
          move.step[step_number] = new_step(pull_from, piece_to);
          move.step[step_number+1] = new_step(victim_from, pull_from);
          moves[move_count++] = move;

          pull_steps &= ~bitboard_at(piece_to);
        }

        victims &= ~bitboard_at(victim_from);
      }

      pullers &= ~bitboard_at(pull_from);
    }
  }
  return move_count;
}

int generate_moves(Position position, Move current_move, Move moves[], int move_count) {
  int new_count = 0;
  Move new_moves[128];
  int current_step_count = step_count(current_move);

  if (current_step_count < STEP_COUNT-1) {
    new_count += generate_push_steps(position, current_move, &new_moves[new_count]);
    new_count += generate_pull_steps(position, current_move, &new_moves[new_count]);
  }
  if (current_step_count < STEP_COUNT) {
    new_count += generate_single_steps(position, current_move, &new_moves[new_count]);
  }

  for (int i = 0; i < new_count; i++) {
    Move move = new_moves[i];
    int move_step_count = step_count(move);

    Position next = position;
    for (int step_number = current_step_count; step_number < move_step_count; step_number++) {
      make_step(&next, move.step[step_number], step_number);
    }

    if (!load_transposition(next.hash)) {
      store_transposition(next.hash, 1);

      Position pass_position = next;
      for (int step_number = move_step_count; step_number < STEP_COUNT; step_number++) {
        move.step[step_number] = PASS_STEP;
        make_step(&pass_position, PASS_STEP, step_number);
      }

      if (!load_transposition(pass_position.hash)) {
        store_transposition(pass_position.hash, 1);
        moves[move_count++] = move;
      }

      if (move_step_count < STEP_COUNT) {
        move_count = generate_moves(next, move, moves, move_count);
      }
    }
  }

  return move_count;
}

Score eval(Position position) {
  Score gold_piece_surplus = __builtin_popcountl(position.pieces[GOLD][ALL]) - __builtin_popcountl(position.pieces[SILVER][ALL]);
  if (position.turn % 2 == 0) {
    return gold_piece_surplus;
  } else {
    return -gold_piece_surplus;
  }
}

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
    if (score > best_score) {
      best_score = score;
    }
    if (score > alpha) {
      alpha = score;
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

void init_move_generation() {
  init_piece_steps();
  init_square_neighbours();
}


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
  char row = 'a' + (from & 7);
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

  Bitboard trap_pieces = position.pieces[colour][ALL] & TRAPS;
  while (trap_pieces) {
    Square trap_piece = first_square(trap_pieces);
    Bitboard trap_square = bitboard_at(trap_piece);

    if (!(position.pieces[colour][ALL] & square_neighbours[trap_piece])) {
      piece = piece_char(colour, type_at_square(position, colour, trap_piece));
      row = 'a' + (trap_piece & 7);
      col = '1' + (trap_piece / 8);
      printf("%c%c%cx ", piece, row, col);

      position.pieces[colour][type_at_square(position, colour, trap_piece)] &= ~trap_square;
      position.pieces[colour][ALL] &= ~trap_square;
    }

    trap_pieces &= ~trap_square;
  }  
}

void print_move(Position position, Move move) {
  for (int i = 0; i < step_count(move); ++i) {
    Step step = move.step[i];
    if (step != PASS_STEP) {
      print_step(position, step);
    }
  }

  printf("\n");
}
