typedef enum Colour { GOLD, SILVER, COLOUR_COUNT } Colour;
typedef enum Type { ALL, RABBIT, CAT, DOG, HORSE, CAMEL, ELEPHANT, TYPE_COUNT } Type;
typedef int Square;
#define SQUARE_COUNT 64

// Indexed so that - a1 = 0, h1 = 7, a8 = 56, h8 = 63
typedef unsigned long long Bitboard;

// Useful bitboards
#define NOT_A_FILE 0xfefefefefefefefeULL
#define NOT_H_FILE 0x7f7f7f7f7f7f7f7fULL
#define NOT_1_RANK 0xffffffffffffff00ULL
#define NOT_8_RANK 0x00ffffffffffffffULL
#define TRAPS      0x0000240000240000ULL
#define EMPTY      0ULL

typedef unsigned long long Hash;

typedef struct Position {
  Bitboard pieces[COLOUR_COUNT][TYPE_COUNT];
  int turn;
  Hash hash;
} Position;

typedef struct Step {
  Square from:8;
  Square to:8;
  Type type:8;
  Colour colour:8;
} Step;

typedef struct Move {
  int step_count;
  Step step[4];
} Move;

typedef int Score;
#define INFINITY 0xffffffff