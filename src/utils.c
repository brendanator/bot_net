#include "print.h"
#include <stdlib.h>
#include <string.h>

int max(int a, int b) {
  return a > b ? a : b;
}

int min(int a, int b) {
  return a < b ? a : b;
}

bool strequal(char *a, char *b) {
  return strcmp(a, b) == 0;
}

void __assert(const char *msg, const char *file, int line, const char *func) {
  fprintf(stderr, "assertion \"%s\" failed: file \"%s\", function \"%s\" line %d\n", msg, file, func, line);
  print_stack_trace();
  printf("testing\n");
  fflush(stdout);
  exit(1);
}
