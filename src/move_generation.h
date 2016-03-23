#define ALL         0
#define RABBIT      1
#define CAT         2
#define DOG         3
#define HORSE       4
#define CAMEL       5
#define ELEPHANT    6

#define GOLD 0
#define SILVER 1

#define NOT_A_FILE  0xfefefefefefefefeULL
#define NOT_H_FILE  0x7f7f7f7f7f7f7f7fULL
#define NOT_1_RANK  0xffffffffffffff00ULL
#define NOT_8_RANK  0x00ffffffffffffffULL
#define TRAPS       0x0000240000240000ULL
#define EMPTY       0ULL

// Indexed so that - a1 = 0, h1 = 7, a8 = 56, h8 = 63
typedef unsigned long long bitboard;

typedef struct position
{
  bitboard pieces[2][7];
  int      turn;
} position;

typedef struct step
{
  char from;
  char to;
  int type;
  int colour;
} step;

typedef struct move
{
  int step_count;
  step step[4];
} move;

void init_move_generation();
void new_game(position *position);
void make_move(position *current, position *next, move move);
void make_step(position *position, step step);
void find_best_move(position *position, move *best_move);
int generate_single_steps(position *position, move *moves);

int first_square(bitboard board);
bitboard bit_at(int n);

void print_bitboard(bitboard bitboard);
void print_position(position position);