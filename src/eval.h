#ifndef EVAL_H
#define EVAL_H

#include "types.h"

void init_eval();

Score eval(Position position);

Score harlog(Position position);
Score piece_alignment(Position position);
Score gaussian_noise(double standard_deviation);

int manhattan_distance(Square a, Square b);

#endif // EVAL_H
