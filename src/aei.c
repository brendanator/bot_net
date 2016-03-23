#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "aei.h"

int parse_message(char *str, char *message, char *options[]) {
  int count = 0;
  char *token = strtok(str, " \r\n");
  strcpy(message, token);
  while (token) {
    token = strtok(NULL, " \r\n");
    options[count++] = token;
  }
  return count-1;
}

void aei() {
  printf("protocol-version 1\n");
  printf("id name bot_net\n");
  printf("id author brendanator\n");
  printf("id version %s\n", version);
  printf("aeiok\n");
}

void parse_steps(step steps[], char *str_steps[], int count) {
  for (int i = 0; i < count; i++) {
    char *str_move = str_steps[i];

    int piece = 16 - strlen( strchr( "-RCDHME--rcdhme-", (int) str_move[0] ) );
    int colour = piece / 8;
    int type = piece & 7;
    int from = (7 ^ ('h' - str_move[1]))  +  ((str_move[2] - '1') * 8);
    int to;
    switch (str_move[3]) {
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

void makemove(position *current, position *next, char *str_steps[], int count) {
  *next = *current;

  step steps[count];
  parse_steps(steps, str_steps, count);
  for (int i = 0; i < count; ++i) {
    make_step(next, steps[i]);
  }

  next->turn++;
}

void go(position *position) {
  if (position->turn == 0) {
    printf("bestmove Ra1 Rb1 Rc1 Rd1 Re1 Rf1 Rg1 Rh1 Ha2 Db2 Cc2 Md2 Ee2 Cf2 Dg2 Hh2\n");
    return;
  } else if (position->turn == 1) {
    printf("bestmove ra8 rb8 rc8 rd8 re8 rf8 rg8 rh8 ha7 db7 cc7 ed7 me7 cf7 dg7 hh7\n");
    return;
  }

  move best_move;
  best_move.step_count = 0;
  find_best_move(position, &best_move);

  char *str_move = (char *)malloc(128 * sizeof(char));
  strcpy(str_move, "bestmove ");
  int s = 9; 

  for (int i = 0; i < best_move.step_count; ++i) {
    step step = best_move.step[i];
    str_move[s++] = "-RCDHME--rcdhme-"[ step.colour * 8 + step.type ]; 
    str_move[s++] = 'a' + (step.from & 7);
    str_move[s++] = '1' + (step.from / 8);
    switch (step.to - step.from) {
      case  8: str_move[s++] = 'n'; break;
      case -8: str_move[s++] = 's'; break;
      case  1: str_move[s++] = 'e'; break;
      case -1: str_move[s++] = 'w'; break;
    }
    str_move[s++] = ' ';
  }

  str_move[s++] = 0;
  printf("%s\n", str_move);
}

int aei_loop(void) {
  position game[500];
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
      printf("readyok\n");
    } else if (strcmp(message_type, "newgame") == 0) {
      new_game(&game[0]);
      turn = 0;
    } else if (strcmp(message_type, "setposition") == 0) {
      // <side> <position>
    } else if (strcmp(message_type, "setoption") == 0) {
      // name <id> [value <x>]
    } else if (strcmp(message_type, "makemove") == 0) {
      makemove(&game[turn], &game[turn+1], options, option_count);
      turn++;
    } else if (strcmp(message_type, "go") == 0) {
      // [ponder]
      go(&game[turn]);
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
