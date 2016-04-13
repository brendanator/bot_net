#ifndef TIME_MANAGEMENT_H
#define TIME_MANAGEMENT_H

#include "types.h"
#include <time.h>

// AEI time management options:
// 0 means unlimited and is the default initial value.
//   * tcmove - The per move time for the game.
//   * tcreserve - The starting reserve time.
//   * tcpercent - The percent of unused time added to the reserve. The initial value is 100 percent. 0 means no unused time is added to the reserve.
//   * tcmax - The maximum reserve time.
//   * tctotal - Time limit for the total length of the game.
//   * tcturns - Maximum number of moves the game can last.
//   * tcturntime - Maximum time a single move can last.
//   * greserve - Amount of reserve time for gold.
//   * sreserve - Amount of reserve time for silver.

void set_tcmove(time_t value);
void set_tcturntime(time_t value);
void set_greserve(time_t value);
void set_sreserve(time_t value);

void start_move_timer(Colour colour);
time_t time_taken();
bool time_up();

#endif