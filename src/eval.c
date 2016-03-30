#include "eval.h"

Score eval(Position position) {
  Score gold_piece_surplus = __builtin_popcountl(position.pieces[GOLD][ALL]) - __builtin_popcountl(position.pieces[SILVER][ALL]);
  if (position.turn % 2 == 0) {
    return gold_piece_surplus;
  } else {
    return -gold_piece_surplus;
  }
}

