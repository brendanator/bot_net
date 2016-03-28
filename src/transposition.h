#include "types.h"

void init_transposition_table();
void reset_transposition_table();

Hash place_update_hash(Hash hash, Colour colour, Type type, Square square);
Hash step_update_hash(Hash hash, Colour colour, Type type, Square from, Square to, int step_number);
Hash capture_update_hash(Hash hash, Colour colour, Type type, Square square);

void store_transposition(Hash hash, Hash value);
Hash load_transposition(Hash hash);