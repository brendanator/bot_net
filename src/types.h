typedef int Boolean;
#define TRUE 1
#define FALSE 0

typedef enum Colour { GOLD, SILVER, COLOUR_COUNT } Colour;
typedef enum Type { ALL, RABBIT, CAT, DOG, HORSE, CAMEL, ELEPHANT, TYPE_COUNT } Type;

typedef unsigned char Square;
#define SQUARE_COUNT 64

// The Direction enum is ordered such that EMPTY_STEP and PASS_STEP are not valid steps
typedef enum Direction { SOUTH, NORTH, WEST, EAST } Direction;

// Indexed so that - a1 = 0, h1 = 7, a8 = 56, h8 = 63
typedef unsigned long long Bitboard;
Bitboard north(Bitboard bitboard);
Bitboard south(Bitboard bitboard);
Bitboard east(Bitboard bitboard);
Bitboard west(Bitboard bitboard);

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

typedef struct PlacePiece {
  Colour colour;
  Type type;
  Square square;
} PlacePiece;

typedef unsigned char Step;
Step new_step(Square from, Square to);
Square step_from(Step step);
Square step_to(Step step);
Direction step_direction(Step step);
#define EMPTY_STEP 0
#define PASS_STEP 0xff
#define STEP_COUNT 4

typedef struct Move {
  Step step[STEP_COUNT];
} Move;
int step_count(Move move);

typedef int Score;
#define INFINITY 0x100000 // 2^20