#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "aei.h"

int parse_message(char *str, char *message, char *options[]) {
  int count = 0;
  char *token = strtok(str, " \n");
  strcpy(message, token);
  while (token) {
    token = strtok(NULL, " \n");
    options[count++] = token;
  }
  return count-1;
}

void aei() {
  puts("protocol-version 1");
  puts("id name bot_net");
  puts("id author brendanator");
  printf("id version %s\n", version);
  puts("aeiok");
}

void parse_steps(Step steps[], char *step_str[], int count) {
  for (int i = 0; i < count; i++) {
    char *move_str = step_str[i];

    int piece = 16 - strlen( strchr( "-RCDHME--rcdhme-", (int) move_str[0] ) );
    Colour colour = piece / 8;
    Type type = piece & 7;
    Square from = (7 ^ ('h' - move_str[1]))  +  ((move_str[2] - '1') * 8);
    Square to;
    switch (move_str[3]) {
      case 'x': to = -1; break;
      case 'n': to = from + 8; break;
      case 's': to = from - 8; break;
      case 'e': to = from + 1; break;
      case 'w': to = from - 1; break;
      default: to = from; from = -1;
    }

    steps[i].from = from;
    steps[i].to = to;
    steps[i].type = type;
    steps[i].colour = colour;
  }
}

void makemove(Position *position, char *step_str[], int count) {
  Step steps[count];
  parse_steps(steps, step_str, count);
  for (int i = 0; i < count; ++i) {
    make_step(position, steps[i]);
  }

  position->turn++;
}

char piece_char(Colour colour, Type type) {
  return "-RCDHME--rcdhme-"[ colour * 8 + type ];
}

void go(Position position) {
  if (position.turn == 0) {
    puts("bestmove Ra1 Rb1 Rc1 Rd1 Re1 Rf1 Rg1 Rh1 Ha2 Db2 Cc2 Md2 Ee2 Cf2 Dg2 Hh2");
    return;
  } else if (position.turn == 1) {
    puts("bestmove ra8 rb8 rc8 rd8 re8 rf8 rg8 rh8 ha7 db7 cc7 ed7 me7 cf7 dg7 hh7");
    return;
  }

  Move best_move = find_best_move(position);

  char move_str[128];
  strcpy(move_str, "bestmove");

  for (int i = 0; i < best_move.step_count; ++i) {
    Step step = best_move.step[i];
    char piece = piece_char(step.colour, step.type); 
    char row = 'a' + (step.from & 7);
    char col = '1' + (step.from / 8);
    char direction;
    switch (step.to - step.from) {
      case  8: direction = 'n'; break;
      case -8: direction = 's'; break;
      case  1: direction = 'e'; break;
      case -1: direction = 'w'; break;
    }

    char step_str[6];
    sprintf(step_str, " %c%c%c%c", piece, row, col, direction);
    strcat(move_str, step_str);

    // Captures piece step
    Bitboard trap_pieces = position.pieces[step.colour][ALL] & TRAPS;
    while (trap_pieces) {
      Square trap_piece = first_square(trap_pieces);
      Bitboard trap_square = bitboard_at(trap_piece);

      if (!(position.pieces[step.colour][ALL] & square_neighbours[trap_piece])) {
        piece = piece_char(step.colour, type_at_square(position, step.colour, trap_piece)); 
        row = 'a' + (trap_square & 7);
        col = '1' + (trap_square / 8);
        sprintf(step_str, " %c%c%cx", piece, row, col);
        strcat(move_str, step_str);
      }

      trap_pieces &= ~trap_square;
    }
  }

  puts(move_str);
}

void aei_loop() {
  Position game[500];
  int turn;

  size_t bufsize = 128;
  char *controller_message = malloc(bufsize * sizeof(char));
  char message_type[16];
  char *options[64];

  for (;;) {
    getline(&controller_message, &bufsize, stdin);
    int option_count = parse_message(controller_message, message_type, options);

    if (strcmp(message_type, "aei") == 0) {
      aei();
    } else if (strcmp(message_type, "isready") == 0) {
      puts("readyok");
    } else if (strcmp(message_type, "newgame") == 0) {
      game[0] = new_game();
      turn = 0;
    } else if (strcmp(message_type, "setposition") == 0) {
      // <side> <position>
    } else if (strcmp(message_type, "setoption") == 0) {
      // name <id> [value <x>]
    } else if (strcmp(message_type, "makemove") == 0) {
      turn++;
      game[turn] = game[turn-1];
      makemove(&game[turn], options, option_count);
    } else if (strcmp(message_type, "go") == 0) {
      // [ponder]
      go(game[turn]);
    } else if (strcmp(message_type, "stop") == 0) {
       // 
    } else if (strcmp(message_type, "quit") == 0) {
      exit(EXIT_SUCCESS);
    } else {
      exit(EXIT_FAILURE);
    }

    fflush(stdout);
  }
}
