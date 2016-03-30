#include "move_generation.h"
#include "transposition.h"

#include <stdlib.h>

Bitboard piece_steps[COLOUR_COUNT][TYPE_COUNT][SQUARE_COUNT];
void init_move_generation() {
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

    if (!(position->pieces[colour][ALL] & square_neighbours(trap_square))) {
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
      Bitboard victims = square_neighbours(push_from) & weaker;

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
      Bitboard victims = square_neighbours(pull_from) & weaker;

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
