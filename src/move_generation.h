typedef enum Type { ALL, RABBIT, CAT, DOG, HORSE, CAMEL, ELEPHANT, TYPE_COUNT } Type;
typedef enum Colour { GOLD, SILVER, COLOUR_COUNT } Colour;
typedef int Square;

// Indexed so that - a1 = 0, h1 = 7, a8 = 56, h8 = 63
typedef unsigned long long Bitboard;

// Useful bitboards
#define NOT_A_FILE 0xfefefefefefefefeULL
#define NOT_H_FILE 0x7f7f7f7f7f7f7f7fULL
#define NOT_1_RANK 0xffffffffffffff00ULL
#define NOT_8_RANK 0x00ffffffffffffffULL
#define TRAPS      0x0000240000240000ULL
#define EMPTY      0ULL

typedef struct Position {
  Bitboard pieces[COLOUR_COUNT][TYPE_COUNT];
  int      turn;
} Position;

typedef struct Step {
  Square from;
  Square to;
  Type type;
  Colour colour;
} Step;

typedef struct Move {
  int step_count;
  Step step[4];
} Move;

void init_move_generation();
Position new_game();

void make_move(Position *position, Move move);
void make_step(Position *position, Step step);

Move find_best_move(Position position);
int generate_single_steps(Position position, Move *moves);
int generate_push_steps(Position position, Move *moves);
int generate_pull_steps(Position position, Move *moves);

Square first_square(Bitboard board);
Type type_at_square(Position position, Colour colour, Square square);
Bitboard bitboard_at(Square square);
Bitboard square_neighbours[64];

void print_bitboard(Bitboard bitboard);
void print_position(Position position);
