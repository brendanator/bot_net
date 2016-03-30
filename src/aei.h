#ifndef AEI_H
#define AEI_H

#include "types.h"

#define version "0.0.1"

void parse_steps(Step steps[], char *step_str[], int count);
int parse_message(char *str, char *message, char *options[]);
void makemove(Position *position, char *step_str[], int count);
void aei_loop();
void go(Position position);

#endif // AEI_H