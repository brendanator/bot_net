#include "transposition.h"

#include <stdlib.h>
#include <string.h>

/*
  Positions are hashed using Zobrist hashing
  Entries are stored using Hyatt's lockless transposition table - https://www.cis.uab.edu/hyatt/hashing.html
*/

Hash piece_hashes[COLOUR_COUNT][TYPE_COUNT][SQUARE_COUNT];
Hash colour_step_hashes[COLOUR_COUNT * STEP_COUNT];

typedef struct TranpositionEntry {
  Hash key;
  Transposition value;
} TranpositionEntry;

#define TRANPOSITION_ENTRY_COUNT 0x100000 // Must be power of 2
TranpositionEntry transposition_table[TRANPOSITION_ENTRY_COUNT];
int INDEX_MASK = TRANPOSITION_ENTRY_COUNT-1;

void init_transposition_table() {
  srand(0);

  for (Colour colour = 0; colour < COLOUR_COUNT; colour++) {
    for (Type type = 0; type < TYPE_COUNT; type++ ) {
      for (Square square = 0; square < SQUARE_COUNT; square++) {
        piece_hashes[colour][type][square] = ((Hash) rand() << 32) ^ ((Hash) rand());
      }
    }

    for (int step = 0; step < STEP_COUNT; step++) {
      colour_step_hashes[colour * STEP_COUNT + step] = ((Hash) rand() << 32) ^ ((Hash) rand());
    }
  }
}

void reset_transposition_table() {
  memset(&transposition_table, 0, sizeof(transposition_table));
}

Hash place_update_hash(Hash hash, Colour colour, Type type, Square square) {
  return hash ^ piece_hashes[colour][type][square];
}

int colour_step_hash(Colour colour, int step_number) {
  int index = (colour * STEP_COUNT + step_number) % 8;
  return colour_step_hashes[index];
}

Hash step_update_hash(Hash hash, Colour colour, Type type, Square from, Square to, int step_number) {
  return hash ^
    piece_hashes[colour][type][from] ^
    piece_hashes[colour][type][to] ^
    colour_step_hash(colour, step_number) ^
    colour_step_hash(colour, step_number+1);
}

Hash pass_update_hash(Hash hash, Colour colour, int step_number) {
  return hash ^
    colour_step_hash(colour, step_number) ^
    colour_step_hash(colour, step_number+1);  
}

Hash capture_update_hash(Hash hash, Colour colour, Type type, Square square) {
  return hash ^ piece_hashes[colour][type][square];
}

Hash xor(Hash hash, Transposition transposition) {
  Hash value;
  memcpy(&value, &transposition, sizeof(value));
  return hash ^ value;
}

void save_transposition(Position position, Transposition transposition) {
  TranpositionEntry entry = { .key = xor(position.hash, transposition), .value = transposition };
  transposition_table[position.hash & INDEX_MASK] = entry;
}

bool load_transposition(Position position, Transposition *transposition) {
  TranpositionEntry entry = transposition_table[position.hash & INDEX_MASK];

  if (xor(entry.key, entry.value) == position.hash) {
    *transposition = entry.value;
    return true;
  } else {
    return false;
  }
}
