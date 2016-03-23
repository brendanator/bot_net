#include "move_generation.h"

#define version "0.0.1"

void parse_steps(step steps[], char *str_steps[], int count);
int parse_message(char *str, char *message, char *options[]);
void makemove(position *current, position *next, char *str_steps[], int count);
int aei_loop(void);
void go(position *position);
