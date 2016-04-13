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

void place_piece(Position *position, PlacePiece piece) {
  position->pieces[piece.colour][piece.type] |= bitboard_at(piece.square);
  position->pieces[piece.colour][ALL] |= bitboard_at(piece.square);

  position->hash = place_update_hash(position->hash, piece.colour, piece.type, piece.square);
}

void make_step(Position *position, Step step, int step_number) {
  if (step == PASS_STEP) {
    position->hash = pass_update_hash(position->hash, step_number);
  } else {
    Square from = step_from(step);
    Square to = step_to(step);
    Colour colour = colour_at_square(*position, from);
    Type type = type_at_square(*position, colour, from);

    position->pieces[colour][type] &= ~bitboard_at(from);
    position->pieces[colour][type] |= bitboard_at(to);
    position->pieces[colour][ALL] &= ~bitboard_at(from);
    position->pieces[colour][ALL] |= bitboard_at(to);

    position->hash = step_update_hash(position->hash, colour, type, from, to, step_number);

    // Piece captured?
    Bitboard trap = square_neighbours(from) & TRAPS;
    if (trap) {
      Square trap_square = first_square(trap);

      if (position->pieces[colour][ALL] & trap && !(position->pieces[colour][ALL] & square_neighbours(trap_square))) {
        Type type = type_at_square(*position, colour, trap_square);
        position->pieces[colour][type] &= ~trap;
        position->pieces[colour][ALL] &= ~trap;

        position->hash = capture_update_hash(position->hash, colour, type, trap_square);
      }
    }
  }

  position->steps++;

  // Check for a winner
  if (turn_colour(position->turn) == GOLD) {
    if (position->pieces[GOLD][RABBIT] & GOLD_GOAL || !position->pieces[SILVER][RABBIT]) {
      position->winner = GOLD_WIN;
    }
  } else {
    if (position->pieces[SILVER][RABBIT] & SILVER_GOAL|| !position->pieces[GOLD][RABBIT]) {
      position->winner = SILVER_WIN;
    }
  }

  if (position->steps == STEP_COUNT) {
    position->turn++;
    position->steps = 0;
  }
}

void make_move(Position *position, Move move) {
  for (int step_number = 0; step_number < move.step_count; step_number++) {
    make_step(position, move.steps.step[step_number], step_number);
  }
}

int generate_pass(Move current_move, Move moves[]) {
  if (current_move.step_count < STEP_COUNT) {
    for (int step_number = current_move.step_count; step_number < STEP_COUNT; step_number++) {
      current_move.steps.step[step_number] = PASS_STEP;
    }
    current_move.step_count = 4;
    moves[0] = current_move;
    return 1;
  } else {
    return 0;
  }
}

int generate_single_steps(Position position, Move current_move, Move moves[]) {
  int move_count = 0;
  Colour colour = turn_colour(position.turn);
  Colour enemy = enemy_colour(colour);
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
        move.steps.step[move.step_count++] = new_step(from, to);
        moves[move_count++] = move;

        steps &= ~bitboard_at(to);
      }

      pieces &= ~bitboard_at(from);
    }
  }

  return move_count;
}

int generate_push_pull_steps(Position position, Move current_move, Move moves[]) {
  int move_count = 0;
  Colour colour = turn_colour(position.turn);
  Colour enemy = enemy_colour(colour);
  Bitboard empty_squares = ~(position.pieces[GOLD][ALL] | position.pieces[SILVER][ALL]);
  Bitboard stronger = position.pieces[enemy][ALL] & ~position.pieces[enemy][RABBIT];
  Bitboard weaker = EMPTY;

  for (Type type = CAT; type <= ELEPHANT; type++) {
    stronger &= ~position.pieces[enemy][type];
    weaker |= position.pieces[enemy][type-1];
    Bitboard unfrozen = all_neighbours(position.pieces[colour][ALL]) | (~all_neighbours(stronger));
    Bitboard attackers = position.pieces[colour][type] & all_neighbours(weaker) & unfrozen;

    while (attackers) {
      Square attack_square = first_square(attackers);
      Bitboard victims = square_neighbours(attack_square) & weaker;

      while (victims) {
        Square victim_square = first_square(victims);

        Bitboard push_steps = empty_squares & piece_steps[enemy][ALL][victim_square];
        while (push_steps) {
          Square victim_to = first_square(push_steps);

          Move move = current_move;
          move.steps.step[move.step_count++] = new_step(victim_square, victim_to);
          move.steps.step[move.step_count++] = new_step(attack_square, victim_square);
          moves[move_count++] = move;

          push_steps &= ~bitboard_at(victim_to);
        }

        Bitboard pull_steps = empty_squares & piece_steps[enemy][ALL][attack_square];
        while (pull_steps) {
          Square piece_to = first_square(pull_steps);

          Move move = current_move;
          move.steps.step[move.step_count++] = new_step(attack_square, piece_to);
          move.steps.step[move.step_count++] = new_step(victim_square, attack_square);
          moves[move_count++] = move;

          pull_steps &= ~bitboard_at(piece_to);
        }

        victims &= ~bitboard_at(victim_square);
      }

      attackers &= ~bitboard_at(attack_square);
    }
  }
  return move_count;
}

int generate_moves(Position position, Move current_move, Move moves[], int move_count) {
  int new_count = 0;
  Move new_moves[128];

  // Make sure this position is not revisted
  Transposition transposition = {};
  save_transposition(position, transposition);

  if (current_move.step_count < STEP_COUNT-1) {
    new_count += generate_push_pull_steps(position, current_move, &new_moves[new_count]);
  }
  if (current_move.step_count < STEP_COUNT) {
    new_count += generate_single_steps(position, current_move, &new_moves[new_count]);
  }

  for (int i = 0; i < new_count; i++) {
    Move move = new_moves[i];

    Position next = position;
    for (int step_number = current_move.step_count; step_number < move.step_count; step_number++) {
      make_step(&next, move.steps.step[step_number], step_number);
    }

    if (!load_transposition(next, &transposition)) {
      save_transposition(next, transposition);
      if (move.step_count) {
        moves[move_count++] = move;
      }

      if (move.step_count < STEP_COUNT) {
        move_count = generate_moves(next, new_moves[i], moves, move_count);
      }
    }
  }

  return move_count;
}

bool is_legal(Position position, Move move) {
  Colour colour = turn_colour(position.turn);
  Colour enemy = enemy_colour(colour);

  Type pushed_type = 0;
  Square pushed_from = -1;
  Type pulling_type = 0;
  Square pulling_to = -1;

  for (int step_number = position.steps; step_number < move.step_count; step_number++) {
    Step step = move.steps.step[step_number];
    Square from = step_from(step);
    Square to = step_to(step);

    if ((position.pieces[GOLD][ALL] | position.pieces[SILVER][ALL]) & bitboard_at(to)) {
      // To square must be empty
      return false;
    }

    if (position.pieces[colour][ALL] & bitboard_at(from)) {
      // Check piece is not frozen
      Bitboard neighbours = square_neighbours(from);
      Type type = type_at_square(position, colour, from);
      if (!(position.pieces[colour][ALL] & neighbours)) {
        for (; type <= ELEPHANT; type++) {
          if (position.pieces[enemy][type] & neighbours) {
            return false;
          }
        }
      }

      if (pushed_type >= RABBIT) { // Incomplete push?
        if (pushed_type >= type || pushed_from != to) {
          return false;
        }
        // Piece cannot take part in another push/pull
        pushed_type = 0;
        pushed_from = -1;
        pulling_type = 0;
        pulling_to = -1;
      } else {
        // Allow a subsequent pull
        pulling_type = type;
        pulling_to = to;
      }
    } else if (position.pieces[enemy][ALL] & bitboard_at(from)) { // Enemy piece being pushed or pulled?
      Type type = type_at_square(position, enemy, from);

      if (type < pulling_type && to == pulling_to) { // Piece was pulled?
        pulling_type = 0;
        pulling_to = -1;
      } else {
        // Piece must be pushed by next step
        pushed_from = to;
        pushed_type = type;
      }
    } else {
      // No piece at from square
      return false;
    }

    // Update the position ready for the next step
    make_step(&position, step, step_number);
  }

  // No outstanding push to be completed
  return pushed_type == 0;
}
