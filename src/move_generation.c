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

Bitboard piece_steps[COLOUR_COUNT][TYPE_COUNT][SQUARE_COUNT];
void init_piece_steps() {
  for (Colour colour = 0; colour < COLOUR_COUNT; colour++) {
    for (Type type = 0; type < TYPE_COUNT; type++) {
      for (Square square = 0; square < SQUARE_COUNT; square++) {
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
  Position position = { .turn = 0, .hash = 0ULL };

  for (Type type = ALL; type <= ELEPHANT; type++) {
    position.pieces[GOLD][type] = EMPTY;
    position.pieces[SILVER][type] = EMPTY;
  }

  return position;
}

Move new_move() {
  Move move = { .step_count = 0 };

  Step pass_step = { .from = 0, .to = 0, .type = 0, .colour = 0 };
  for (int step_number = 0; step_number < 4; step_number++) {
    move.step[step_number] = pass_step;
  }

  return move;
}

Bitboard all_neighbours(Bitboard board) {
  return
    (board & NOT_8_RANK) << 8 | // north
    (board & NOT_1_RANK) >> 8 | // south
    (board & NOT_H_FILE) << 1 | // east
    (board & NOT_A_FILE) >> 1;  // west
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

void place_piece(Position *position, PlacePiece piece) {
  position->pieces[piece.colour][piece.type] |= bitboard_at(piece.square);
  position->pieces[piece.colour][ALL] |= bitboard_at(piece.square);

  position->hash = place_update_hash(position->hash, piece.colour, piece.type, piece.square);
}

void make_step(Position *position, Step step, int step_number) {
  position->pieces[step.colour][step.type] &= ~bitboard_at(step.from);
  position->pieces[step.colour][step.type] |= bitboard_at(step.to);
  position->pieces[step.colour][ALL] &= ~bitboard_at(step.from);
  position->pieces[step.colour][ALL] |= bitboard_at(step.to);

  position->hash = step_update_hash(position->hash, step.colour, step.type, step.from, step.to, step_number);

  Bitboard trap_pieces = position->pieces[step.colour][ALL] & TRAPS;
  while (trap_pieces) {
    Square trap_square = first_square(trap_pieces);
    Bitboard trap = bitboard_at(trap_square);

    if (!(position->pieces[step.colour][ALL] & square_neighbours[trap_square])) {
      Type type = type_at_square(*position, step.colour, trap_square);
      position->pieces[step.colour][type] &= ~trap;
      position->pieces[step.colour][ALL] &= ~trap;

      position->hash = capture_update_hash(position->hash, step.colour, type, trap_square);

      break;
    }

    trap_pieces &= ~trap;
  }
}

void make_move(Position *position, Move move) {
  for (int step_number = 0; step_number < move.step_count; step_number++) {
    make_step(position, move.step[step_number], step_number);
  }
}

int generate_single_steps(Position position, Move current_move, Move moves[]) {
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

        Move move = current_move;

        move.step[move.step_count].from = from;
        move.step[move.step_count].to = to;
        move.step[move.step_count].type = type;
        move.step[move.step_count].colour = colour;

        move.step_count++;
        moves[move_count] = move;

        move_count++;

        steps &= ~bitboard_at(to);
      }

      pieces &= ~bitboard_at(from);
    }
  }

  return move_count;
}

int generate_push_steps(Position position, Move current_move, Move moves[]) {
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

          Move move = current_move;

          move.step[move.step_count].from = victim_from;
          move.step[move.step_count].to = victim_to;
          move.step[move.step_count].type = victim_type;
          move.step[move.step_count].colour = enemy;

          move.step[move.step_count + 1].from = push_from;
          move.step[move.step_count + 1].to = victim_from;
          move.step[move.step_count + 1].type = type;
          move.step[move.step_count + 1].colour = colour;

          move.step_count += 2;
          moves[move_count] = move;

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

int generate_pull_steps(Position position, Move current_move, Move moves[]) {
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

          Move move = current_move;

          move.step[move.step_count].from = pull_from;
          move.step[move.step_count].to = piece_to;
          move.step[move.step_count].type = type;
          move.step[move.step_count].colour = colour;

          move.step[move.step_count + 1].from = victim_from;
          move.step[move.step_count + 1].to = pull_from;
          move.step[move.step_count + 1].type = victim_type;
          move.step[move.step_count + 1].colour = enemy;

          move.step_count += 2;
          moves[move_count] = move;

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

int generate_moves(Position position, Move current_move, Move moves[], int move_count) {
  int new_count = 0;
  Move new_moves[128];

  if (current_move.step_count < 3) {
    new_count += generate_push_steps(position, current_move, &new_moves[new_count]);
    new_count += generate_pull_steps(position, current_move, &new_moves[new_count]);
  }
  if (current_move.step_count < 4) {
    new_count += generate_single_steps(position, current_move, &new_moves[new_count]);
  }

  for (int i = 0; i < new_count; i++) {
    Move move = new_moves[i];

    Position next = position;
    for (int step_number = current_move.step_count; step_number < move.step_count; step_number++) {
      make_step(&next, move.step[step_number], step_number);
    }

    if (!load_transposition(next.hash)) {
      store_transposition(next.hash, 1);

      Position pass_position = next;
      for (int step_number = move.step_count; step_number <= 4; step_number++) {
        make_step(&pass_position, move.step[step_number], step_number);
      }

      if (!load_transposition(pass_position.hash)) {
        store_transposition(pass_position.hash, 1);
        moves[move_count++] = move;
      }

      if (move.step_count < 4) {
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

void print_short_position(Position position) {
  printf("[");
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

void print_move(Position position, Move move) {
  char move_str[128] = "";

  for (int i = 0; i < move.step_count; ++i) {
    Step step = move.step[i];
    char piece = piece_char(step.colour, step.type);
    char row = 'a' + (step.from & 7);
    char col = '1' + (step.from / 8);
    char direction;
    switch (step.to - step.from) {
      case  8: direction = 'n'; break;
      case -8: direction = 's'; break;
      case  1: direction = 'e'; break;
      case -1: direction = 'w'; break;
    }

    char step_str[6];
    sprintf(step_str, "%c%c%c%c ", piece, row, col, direction);
    strcat(move_str, step_str);

    // Captures piece step
    position.pieces[step.colour][step.type] &= ~bitboard_at(step.from);
    position.pieces[step.colour][step.type] |= bitboard_at(step.to);
    position.pieces[step.colour][ALL] &= ~bitboard_at(step.from);
    position.pieces[step.colour][ALL] |= bitboard_at(step.to);

    Bitboard trap_pieces = position.pieces[step.colour][ALL] & TRAPS;
    while (trap_pieces) {
      Square trap_piece = first_square(trap_pieces);
      Bitboard trap_square = bitboard_at(trap_piece);

      if (!(position.pieces[step.colour][ALL] & square_neighbours[trap_piece])) {
        piece = piece_char(step.colour, type_at_square(position, step.colour, trap_piece));
        row = 'a' + (trap_piece & 7);
        col = '1' + (trap_piece / 8);
        sprintf(step_str, "%c%c%cx ", piece, row, col);
        strcat(move_str, step_str);

        position.pieces[step.colour][type_at_square(position, step.colour, trap_piece)] &= ~trap_square;
        position.pieces[step.colour][ALL] &= ~trap_square;
      }

      trap_pieces &= ~trap_square;
    }
  }

  puts(move_str);
}
