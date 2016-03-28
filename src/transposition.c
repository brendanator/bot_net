#include "transposition.h"

#include <stdlib.h>

/*
  Positions are hashed using Zobrist hashing
  Entries are stored using Hyatt's lockless transposition table - https://www.cis.uab.edu/hyatt/hashing.html
*/

Hash piece_hashes[COLOUR_COUNT][TYPE_COUNT][SQUARE_COUNT];
Hash colour_step_hashes[COLOUR_COUNT * 4]; // 4 steps per move 

typedef struct TranpositionEntry {
  Hash key;
  Hash value;
} TranpositionEntry;

#define TRANPOSITION_ENTRY_COUNT 0x100000 // Must be power of 2
TranpositionEntry transposition_table[TRANPOSITION_ENTRY_COUNT];
int TABLE_MASK = TRANPOSITION_ENTRY_COUNT-1;

void init_transposition_table() {
  srand(0);

  for (Colour colour = 0; colour < COLOUR_COUNT; colour++) {
    for (Type type = 0; type < TYPE_COUNT; type++ ) {
      for (Square square = 0; square < SQUARE_COUNT; square++) {
        piece_hashes[colour][type][square] = ((Hash) rand() << 32) ^ ((Hash) rand());
      }
    }

    for (int step = 0; step < 4; step++) {
      colour_step_hashes[colour * 4 + step] = ((Hash) rand() << 32) ^ ((Hash) rand());
    }
  }
}

void reset_transposition_table() {
  TranpositionEntry empty_entry = { .key = 0, .value = 0 };
  for (int i = 0; i < TRANPOSITION_ENTRY_COUNT; i++) {
    transposition_table[i] = empty_entry;
  }  
}

Hash place_update_hash(Hash hash, Colour colour, Type type, Square square) {
  return hash ^ piece_hashes[colour][type][square];
}

int colour_step_hash(Colour colour, int step_number) {
  int index = (colour*4 + step_number) & 7;
  return colour_step_hashes[index];
}

Hash step_update_hash(Hash hash, Colour colour, Type type, Square from, Square to, int step_number) {
  return hash ^
    piece_hashes[colour][type][from] ^
    piece_hashes[colour][type][to] ^
    colour_step_hash(colour, step_number) ^
    colour_step_hash(colour, step_number+1);
}

Hash capture_update_hash(Hash hash, Colour colour, Type type, Square square) {
  return hash ^ piece_hashes[colour][type][square];
}

void store_transposition(Hash hash, Hash value) {
  TranpositionEntry entry;
  entry.key = hash ^ value;
  entry.value = value;

  int index = hash & TABLE_MASK;
  transposition_table[index] = entry;
}

Hash load_transposition(Hash hash) {
  int index = hash & TABLE_MASK;
  TranpositionEntry entry = transposition_table[index];

  if ((entry.key ^ entry.value) == hash) {
    return entry.value;
  } else {
    return 0;
  }
}
