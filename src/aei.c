#include "aei.h"
#include "search.h"
#include "move_generation.h"
#include "transposition.h"
#include "print.h"
#include "time_management.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int parse_message(char *str, char *message, char *options[]) {
  int count = 0;
  char *token = strtok(str, " \n");

  if (token) {
    strcpy(message, token);
  } else {
    strcpy(message, "quit");
  }

  while (token) {
    token = strtok(NULL, " \n");
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

void setposition(Position *position, char *player, char *position_str) {
  if (player[0] == 'g') {
    position->turn = 2;
  } else {
    position->turn = 3;
    for (int step_number = 0; step_number < STEP_COUNT; step_number++) {
      position->hash = pass_update_hash(position->hash, step_number);
    }
  }

  for (int i = 0; i < 63; i++) {
    if (position_str[i+1] && position_str[i+1] != ' ') {
      int piece_index = 16 - strlen(strchr("-RCDHME--rcdhme-", position_str[i+1]));
      Colour colour = (piece_index/8) & 1;
      Type type = piece_index & 7;
      Square square = 8*(7 - i/8) + (i%8);
      PlacePiece piece = { .colour = colour, .type = type, .square = square };
      place_piece(position, piece);
    }
  }
}

void makemove(Position *position, char *step_str[], int count) {
  int step_count = 0;
  bool placing_pieces = false;

  for (int i = 0; i < count; i++) {
    char *move_str = step_str[i];

    Square from = (7 ^ ('h' - move_str[1]))  +  ((move_str[2] - '1') * 8);
    Square to = 0;
    bool is_step = true;
    switch (move_str[3]) {
      case 'n': to = from + 8; break;
      case 's': to = from - 8; break;
      case 'e': to = from + 1; break;
      case 'w': to = from - 1; break;
      case 'x': continue;
      default: is_step = false;
    }

    if (is_step) {
      make_step(position, new_step(from, to), step_count++);
    } else {
      int piece_index = 16 - strlen(strchr("-RCDHME--rcdhme-", move_str[0]));
      Colour colour = piece_index / 8;
      Type type = piece_index & 7;
      PlacePiece piece = { .colour = colour, .type = type, .square = from };
      place_piece(position, piece);
      placing_pieces = true;
    }
  }

  // Complete the turn
  for (int step_number = step_count; step_number < STEP_COUNT; step_number++) {
    make_step(position, PASS_STEP, step_number);
  }
  if (placing_pieces) {
    position->winner = 0;
  }
}

void go(Position position) {
  if (position.turn == 0) {
    printf("bestmove Ra1 Rb1 Rc1 Rf1 Rg1 Rh1 Ra2 Rh2 De1 Hb2 Cc2 Md2 Ee2 Cf2 Hg2 Dd1\n");
    return;
  } else if (position.turn == 1) {
    printf("bestmove ra8 rb8 rc8 ra7 rh7 rf8 rg8 rh8 dd8 hb7 cc7 ed7 me7 cf7 hg7 de8\n");
    return;
  }

  Move best_move = find_best_move(position);

  printf("bestmove ");
  print_move(position, best_move);
}

void setoption(char *name, char *value) {
  if (strequal(name, "tcmove")) {
    set_tcmove(strtol(value, NULL, 10));
  } else if (strequal(name, "tcturntime")) {
    set_tcturntime(strtol(value, NULL, 10));
  } else if (strequal(name, "greserve")) {
    set_greserve(strtol(value, NULL, 10));
  } else if (strequal(name, "sreserve")) {
    set_sreserve(strtol(value, NULL, 10));
  }
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

    if (strequal(message_type, "aei")) {
      aei();
    } else if (strequal(message_type, "isready")) {
      printf("readyok\n");
    } else if (strequal(message_type, "newgame")) {
      game[0] = new_game();
      turn = 0;
      reset_transposition_table();
    } else if (strequal(message_type, "setposition")) {
      setposition(&game[0], options[0], options[1]);
    } else if (strequal(message_type, "setoption")) {
      setoption(options[1], options[3]);
    } else if (strequal(message_type, "makemove")) {
      turn++;
      game[turn] = game[turn-1];
      makemove(&game[turn], options, option_count);
    } else if (strequal(message_type, "go")) {
      // [ponder]
      go(game[turn]);
    } else if (strequal(message_type, "stop")) {
       //
    } else if (strequal(message_type, "quit")) {
      exit(EXIT_SUCCESS);
    } else {
      exit(EXIT_FAILURE);
    }

    fflush(stdout);
  }
}
