#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#include "types.h"

void init_transposition_table();
void reset_transposition_table();

Hash place_update_hash(Hash hash, Colour colour, Type type, Square square);
Hash step_update_hash(Hash hash, Colour colour, Type type, Square from, Square to, int step_number);
Hash pass_update_hash(Hash hash, Colour colour, int step_number);
Hash capture_update_hash(Hash hash, Colour colour, Type type, Square square);

typedef enum ScoreBound { NONE, LOWER_BOUND, EXACT, UPPER_BOUND } ScoreBound;

typedef struct __attribute__((__packed__)) Transposition {
  Move best_move;
  Score score:21;
  ScoreBound bound:2;
  int depth:9;
} Transposition;

void save_transposition(Position position, Transposition transposition);
bool load_transposition(Position position, Transposition *transposition);

#endif //TRANSPOSITION_H
