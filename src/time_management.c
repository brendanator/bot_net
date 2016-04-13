#include "time_management.h"
#include "utils.h"

// AEI options
time_t tcmove;
time_t tcturntime;
time_t greserve;
time_t sreserve;

void set_tcmove(time_t value) {
  tcmove = value;
}

void set_tcturntime(time_t value) {
  tcturntime = value;
}

void set_greserve(time_t value) {
  greserve = value;
}

void set_sreserve(time_t value) {
  sreserve = value;
}

// Search times
time_t start_time;
time_t end_time;

void start_move_timer(Colour colour) {
  start_time = time(NULL);

  time_t reserve;
  if (colour == GOLD) {
    reserve = greserve;
  } else {
    reserve = sreserve;
  }

  if (tcturntime) {
    end_time = start_time + min(reserve, tcturntime);
  } else {
    end_time = start_time + reserve;
  }
}

time_t time_taken() {
  return difftime(time(NULL), start_time);
}

time_t remaining_time() {
  return difftime(end_time, time(NULL));
}

bool time_up() {
  if (tcmove) {
    return remaining_time() < 3;
  } else {
    return false;
  }
}
